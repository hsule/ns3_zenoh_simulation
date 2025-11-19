#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import re
import sys
import argparse
import json5
from pathlib import Path
from datetime import datetime
import csv
from collections import deque, defaultdict

# ---------------- Utilities ----------------

ANSI_RE = re.compile(r"\x1b\[[0-9;]*[A-Za-z]")
ISO_RE = re.compile(r"(?P<iso>\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d+Z)")

def strip_ansi(s: str) -> str:
    return ANSI_RE.sub("", s)

def iso_to_ns(iso: str) -> int:
    return int(datetime.fromisoformat(iso.replace("Z","+00:00")).timestamp() * 1_000_000_000)

def hexlist_to_bytes(hex_list_str: str) -> bytes:
    # 支援 "3d, 01, 0d, ..." 或 "3d,01,0d" 等
    parts = [p.strip().strip(",") for p in hex_list_str.split()]
    clean = [p[:-1] if p.endswith(",") else p for p in parts]
    return bytes(int(x,16) for x in clean if x)

def normalize_topic(s: str) -> str:
    # 讓 "/zenoh-rs-pub" 與 "demo/example/zenoh-rs-pub" 能 endswith 比對
    return s.strip().lstrip("/")

def load_cfg(cfg_path: Path):
    cfg = json5.load(open(cfg_path, "r", encoding="utf-8"))
    zid2node = {}
    nodes = cfg.get("nodes", {})
    for nid, ncfg in nodes.items():
        zc = ncfg.get("zid") or {}
        if zc.get("set"):
            z = zc.get("value")
            if isinstance(z, str) and len(z) == 32:
                zid2node[z] = nid
    return cfg, zid2node

def build_graph(cfg: dict):
    g = defaultdict(list)
    for link in cfg.get("links", []):
        a, b = str(link.get("a")), str(link.get("b"))
        if a and b:
            g[a].append(b)
            g[b].append(a)
    return g

def find_sub_nodes(cfg: dict):
    subs = []
    for nid, ncfg in cfg.get("nodes", {}).items():
        if ncfg.get("row") == "sub":
            subs.append(str(nid))
    return subs

# ---------------- Regex (multiline blocks) ----------------

# subscriber OR router：transport TRACE 單行（確保 ts_ns 與 recv_ns 同筆）
RE_TRANSPORT_FRAME = re.compile(
    r"""(?P<iso>\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d+Z).*?
        Received:\s*TransportMessage\s*\{.*?
        payload:\s*\[(?P<payload>(?:[0-9a-f]{2}(?:,\s*)?)+)\]
    """,
    re.IGNORECASE | re.DOTALL | re.VERBOSE,
)

# subscriber OR router：api::session recv Push（可抓 publisher_zid / suffix / payload）
RE_PUSH_BLOCK = re.compile(
    r"""
    (?P<iso>\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d+Z).*?
    recv\s+.*?Push\s+.*?\{.*?
      (?:suffix:\s*"(?P<suffix>[^"]+?)".*?)?
      payload:\s*Put\(
        (?:.*?timestamp:\s*Some\([^/]+/(?P<publisher_zid>[0-9a-f]{32})\))?
        .*?payload:\s*ZBuf\s*\{\s*slices:\s*\[\[(?P<payload>(?:[0-9a-f]{2}(?:,\s*)?)+)\]
    """,
    re.IGNORECASE | re.DOTALL | re.VERBOSE,
)

# payload 內容：[ seq ] ts_ns=...
RE_SEQ_TS = re.compile(rb"""\[\s*(?P<seq>\d+)\s*\]\s*ts_ns=(?P<ts>\d{15,20})""")

# router(3)：Face{idx, ZID} Route data for res <topic>
RE_ROUTER_FACE_ROUTE = re.compile(
    r"""(?P<iso>\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d+Z).*?
        Face\{\s*\d+\s*,\s*(?P<zid>[0-9a-f]{32})\s*\}.*?
        Route\s+data\s+for\s+res\s+(?P<res>\S+)
    """,
    re.IGNORECASE | re.DOTALL | re.VERBOSE,
)

# ---------------- Parsers ----------------

def parse_transport_frames(file_text: str):
    """
    從 subscriber 的 transport TRACE 取得：
    seq, ts_ns（payload 解碼）, recv_ns（行首 ISO）
    → 確保同一筆 frame
    """
    out = []
    for m in RE_TRANSPORT_FRAME.finditer(file_text):
        recv_ns = iso_to_ns(m.group("iso"))
        payload_hex = m.group("payload")
        try:
            payload_bytes = hexlist_to_bytes(payload_hex)
        except Exception:
            continue
        st = RE_SEQ_TS.search(payload_bytes)
        if not st:
            continue
        out.append({
            "seq": int(st.group("seq")),
            "ts_ns": int(st.group("ts")),
            "ns": recv_ns,
        })
    return out

def parse_push_blocks(file_text: str):
    """
    解析 api::session recv Push（subscriber 或 router）：
    可能取得 publisher_zid / suffix / seq/ts_ns（payload 解碼）
    """
    out = []
    for m in RE_PUSH_BLOCK.finditer(file_text):
        ns = iso_to_ns(m.group("iso"))
        suffix = m.group("suffix") or ""
        pub_zid = m.group("publisher_zid") or ""
        payload_hex = m.group("payload")
        seq, ts_ns = None, None
        try:
            payload_bytes = hexlist_to_bytes(payload_hex)
            st = RE_SEQ_TS.search(payload_bytes)
            if st:
                seq = int(st.group("seq"))
                ts_ns = int(st.group("ts"))
        except Exception:
            pass
        out.append({
            "ns": ns,
            "topic": suffix,        # 可能是 "/zenoh-rs-pub"
            "seq": seq,             # 可能為 None（某些版本沒帶 payload？）
            "ts_ns": ts_ns,         # 可能為 None
            "publisher_zid": pub_zid,
        })
    return out

def parse_router_faces(file_text: str):
    """router(3)：Face{..,ZID} Route data for res ... → list[{ns,res,zid}]"""
    items = []
    for m in RE_ROUTER_FACE_ROUTE.finditer(file_text):
        items.append({
            "ns": iso_to_ns(m.group("iso")),
            "res": m.group("res"),
            "zid": m.group("zid"),
        })
    items.sort(key=lambda x: x["ns"])
    return items

# ---------------- Correlators ----------------

def match_seq_router_via(router_push, router_faces, face_window_ns=500_000_000, topic_hint=""):
    """
    用 router(3) 的 recv Push（含 seq）去找最近且較早的 Face{..}（在 face_window 內）：
    回傳 dict: seq -> {"via_zid","via_ns"}。
    比對 topic 使用 endswith 容錯（suffix vs res）。
    """
    faces_by_res = defaultdict(list)
    for it in router_faces:
        faces_by_res[it["res"]].append(it)

    def res_cands(topic):
        cand = []
        if topic:
            cand.append(topic)
            cand.append(normalize_topic(topic))
        if topic_hint:
            cand.append(topic_hint)
            cand.append(normalize_topic(topic_hint))
        # 去重保持順序
        seen = set()
        out = []
        for c in cand:
            if c not in seen:
                out.append(c); seen.add(c)
        return out

    seq_map = {}
    for rp in router_push:
        if rp["seq"] is None:
            continue
        ns = rp["ns"]
        best = None
        best_dt = None
        for key in list(faces_by_res.keys()):
            # endswith 容錯：/zenoh-rs-pub vs demo/example/zenoh-rs-pub
            if key.endswith(normalize_topic(rp["topic"])) or normalize_topic(key).endswith(normalize_topic(rp["topic"])) \
               or key.endswith(normalize_topic(topic_hint)) or normalize_topic(key).endswith(normalize_topic(topic_hint)):
                for f in faces_by_res[key]:
                    if f["ns"] <= ns:
                        dt = ns - f["ns"]
                        if dt <= face_window_ns and (best is None or dt < best_dt):
                            best = f; best_dt = dt
        if best:
            seq_map[rp["seq"]] = {"via_zid": best["zid"], "via_ns": best["ns"]}
    return seq_map

def match_time_router_via(sub_event_ns, router_faces, topic, window_ns=1_000_000_000):
    """備援：僅用時間窗 + topic endswith 來找最近且較早的 Face"""
    best = None
    best_dt = None
    norm = normalize_topic(topic)
    for f in router_faces:
        res_norm = normalize_topic(f["res"])
        ok = (f["res"] == topic) or res_norm.endswith(norm) or norm.endswith(res_norm)
        if not ok:
            continue
        if f["ns"] <= sub_event_ns:
            dt = sub_event_ns - f["ns"]
            if dt <= window_ns and (best is None or dt < best_dt):
                best = f; best_dt = dt
    return best

# ---------------- Path hint (BFS over links) ----------------

def bfs_path(graph, start, goal):
    """單源 BFS 回傳一條 path（list of node string）。找不到回 None"""
    if start == goal:
        return [start]
    q = deque([start])
    prev = {start: None}
    while q:
        u = q.popleft()
        for v in graph.get(u, []):
            if v not in prev:
                prev[v] = u
                if v == goal:
                    # reconstruct
                    path = [v]
                    while u is not None:
                        path.append(u)
                        u = prev[u]
                    return list(reversed(path))
                q.append(v)
    return None

def path_hint_from_graph(graph, pub_node, via_node, sub_nodes, must_pass="3"):
    """
    從 pub_node 到各 sub_node 做 BFS，找包含 "... via_node -> must_pass ..." 的路徑並回傳第一條。
    若找不到則回空字串。
    """
    for s in sub_nodes:
        p = bfs_path(graph, pub_node, s)
        if not p: 
            continue
        if must_pass in p and via_node in p:
            i = p.index(must_pass)
            if i > 0 and p[i-1] == via_node:
                return "-".join(p)
    return ""

# ---------------- Main ----------------

def main():
    ap = argparse.ArgumentParser(description="Analyze zenoh subscriber delay & path (seq-level precise matching).")
    ap.add_argument("--config", required=True, help="NETWORK_CONFIG.json5")
    ap.add_argument("--sublog", required=True, help="subscriber log (e.g., zenohd_5.log)")
    ap.add_argument("--routerlog", help="router(3) log (optional)")
    ap.add_argument("--topic", required=True, help="topic as appears in router log (e.g., demo/example/zenoh-rs-pub)")
    ap.add_argument("--out", default="result.csv", help="output CSV")
    ap.add_argument("--face_window_ms", type=int, default=500, help="router Face matching window for seq->via (ms)")
    ap.add_argument("--time_window_ms", type=int, default=1000, help="fallback time-based matching window (ms)")
    args = ap.parse_args()

    cfg, zid2node = load_cfg(Path(args.config))
    graph = build_graph(cfg)
    sub_nodes = find_sub_nodes(cfg)

    # ---------- Subscriber parsing ----------
    sub_text = strip_ansi(Path(args.sublog).read_text(encoding="utf-8", errors="ignore"))
    sub_lines = sub_text.splitlines()

    # ❶ 用 transport frame 取得「同一筆」的 seq+ts_ns+recv_ns
    sub_frames = parse_transport_frames(sub_text)

    # ❷ 用 recv Push 取得 publisher_zid / topic suffix（用 seq 合併）
    sub_push = parse_push_blocks(sub_text)
    # 建立 base events 以 seq 為 key
    base = {}
    for fr in sub_frames:
        base[fr["seq"]] = {
            "seq": fr["seq"],
            "ts_ns": fr["ts_ns"],
            "recv_ns": fr["ns"],
            "publisher_zid": "",
            "topic": "",   # 可能之後補 suffix
        }
    # 用 recv Push 補充 publisher_zid / topic
    for pb in sub_push:
        if pb["seq"] is None:
            continue
        if pb["seq"] in base:
            if pb.get("publisher_zid"):
                base[pb["seq"]]["publisher_zid"] = pb["publisher_zid"]
            if pb.get("topic"):
                base[pb["seq"]]["topic"] = pb["topic"]

    if not base:
        print("[WARN] subscriber log 沒解析到任何 transport frames；請確認有 TRACE 與 payload")
        Path(args.out).write_text("", encoding="utf-8")
        print(f"[OK] wrote {args.out} (0 rows)")
        return

    # ---------- Router parsing (optional) ----------
    router_faces = []
    router_push = []
    seq_to_via = {}
    if args.routerlog:
        r_text = strip_ansi(Path(args.routerlog).read_text(encoding="utf-8", errors="ignore"))
        router_faces = parse_router_faces(r_text)
        router_push  = parse_push_blocks(r_text)  # 內含 seq 與 ns
        # 用 router 的 recv Push 與 Face 先建立 seq -> via_zid
        seq_to_via = match_seq_router_via(
            router_push,
            router_faces,
            face_window_ns=args.face_window_ms * 1_000_000,
            topic_hint=args.topic
        )

    # ---------- Emit CSV ----------
    fields = [
        "seq","topic","ts_ns","recv_ns","delay_ms",
        "publisher_zid","publisher_node",
        "via_zid","via_node","path_hint","match_mode"
    ]
    rows = []

    for seq in sorted(base.keys()):
        ev = base[seq]
        delay_ms = (ev["recv_ns"] - ev["ts_ns"]) / 1e6
        publisher_node = zid2node.get(ev["publisher_zid"], "") if ev["publisher_zid"] else ""

        via_zid, via_node, path_hint, match_mode = "", "", "", "none"

        # 先用 seq 精準對齊
        if seq in seq_to_via:
            via_zid = seq_to_via[seq]["via_zid"]
            via_node = zid2node.get(via_zid, "")
            match_mode = "seq"
        # 退回時間窗 + topic
        elif router_faces:
            best = match_time_router_via(ev["recv_ns"], router_faces, args.topic, window_ns=args.time_window_ms*1_000_000)
            if best:
                via_zid = best["zid"]
                via_node = zid2node.get(via_zid, "")
                match_mode = "time"

        # 自動用 links 組 path（包含 ... via_node -> 3 ...）
        if via_node:
            path_hint = path_hint_from_graph(graph, publisher_node, via_node, sub_nodes, must_pass="3")

        rows.append({
            "seq": seq,
            "topic": ev.get("topic",""),
            "ts_ns": ev["ts_ns"],
            "recv_ns": ev["recv_ns"],
            "delay_ms": f"{delay_ms:.3f}",
            "publisher_zid": ev.get("publisher_zid",""),
            "publisher_node": publisher_node,
            "via_zid": via_zid,
            "via_node": via_node,
            "path_hint": path_hint,
            "match_mode": match_mode,
        })

    with open(args.out, "w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=fields)
        w.writeheader()
        w.writerows(rows)

    # ---------- Drop stats ----------
    seqs = [r["seq"] for r in rows]
    if seqs:
        s = sorted(seqs)
        missing = []
        for a, b in zip(s, s[1:]):
            if b > a + 1:
                missing.extend(range(a+1, b))
        total = s[-1] - s[0] + 1
        drop = total - len(s)
        rate = (drop/total)*100.0 if total>0 else 0.0
        print(f"[OK] wrote {args.out} ({len(rows)} rows)")
        print(f"[INFO] seq range: {s[0]} ~ {s[-1]}, recv: {len(seqs)}, missing: {len(missing)}, drop: {rate:.2f}%")
        if missing:
            head = ", ".join(map(str, missing[:20]))
            more = f" … (total {len(missing)})" if len(missing) > 20 else ""
            print(f"[INFO] missing seqs (first 20): {head}{more}")
    else:
        print(f"[OK] wrote {args.out} (0 rows)")

if __name__ == "__main__":
    main()
