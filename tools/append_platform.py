#!/usr/bin/env python3
import argparse, json, os
from typing import Any, Dict, List, Tuple

def parse_args():
    p = argparse.ArgumentParser()
    p.add_argument("--index", default="docs/package_dadamachines_ctag_tbd_index.json",
                   help="Pfad zum bestehenden Index (Default: docs/...)")
    p.add_argument("--version", required=True, help="Version ohne v, z.B. 1.0.2")
    p.add_argument("--base-url", required=True,
                   help="Basis-URL zu den Release-Assets, z.B. https://github.com/<ORG>/<REPO>/releases/download/v1.0.2/")
    p.add_argument("--esp32-zip", required=True)
    p.add_argument("--esp32-size", required=True)
    p.add_argument("--esp32-sha", required=True)
    p.add_argument("--rp2040-zip", required=True)
    p.add_argument("--rp2040-size", required=True)
    p.add_argument("--rp2040-sha", required=True)
    p.add_argument("--size-as-string", action="store_true",
                   help="Size als String schreiben (falls der existierende Index so ist)")
    p.add_argument("--package-name", default="dadamachines_ctag_tbd")
    p.add_argument("--maintainer", default="Efratsy")
    p.add_argument("--website", default="https://github.com/Efratsy/dadamachines_ctag_tbd_arduino_package")
    p.add_argument("--help-url", default="https://github.com/Efratsy/dadamachines_ctag_tbd_arduino_package/wiki")
    return p.parse_args()

def to_size(val: str, as_string: bool):
    return val if as_string else int(val)

def load_index(path: str) -> Dict[str, Any]:
    if os.path.exists(path):
        with open(path, "r", encoding="utf-8") as f:
            return json.load(f)
    return {"packages": []}

def find_or_create_package(idx: Dict[str, Any], name: str, maintainer: str, website: str, help_url: str):
    for pkg in idx.get("packages", []):
        if pkg.get("name") == name:
            return pkg
    pkg = {
        "name": name,
        "maintainer": maintainer,
        "websiteURL": website,
        "email": "",
        "help": {"online": help_url},
        "platforms": [],
        "tools": []
    }
    idx.setdefault("packages", []).append(pkg)
    return pkg

def upsert_platform(pkg: Dict[str, Any], entry: Dict[str, Any]):
    plats: List[Dict[str, Any]] = pkg.setdefault("platforms", [])
    for i, p in enumerate(plats):
        if p.get("architecture") == entry["architecture"] and p.get("version") == entry["version"]:
            plats[i] = entry
            return
    plats.append(entry)

def semver_tuple(v: str) -> Tuple[int,int,int]:
    parts = v.split(".")
    parts += ["0"]*(3-len(parts))
    return tuple(int(x) for x in parts[:3])

def sort_platforms(pkg: Dict[str, Any]):
    pkg["platforms"].sort(
        key=lambda p: (p.get("architecture",""), semver_tuple(p.get("version","0.0.0"))),
        reverse=True
    )

def main():
    a = parse_args()
    idx = load_index(a.index)
    pkg = find_or_create_package(idx, a.package_name, a.maintainer, a.website, a.help_url)

    esp32 = {
        "name": "dadamachines CTAG TBD",
        "architecture": "esp32",
        "version": a.version,
        "category": "Contributed",
        "url": a.base_url + a.esp32_zip,
        "archiveFileName": a.esp32_zip,
        "checksum": f"SHA-256:{a.esp32_sha}",
        "size": to_size(a.esp32_size, a.size_as_string),
        "boards": [{"name": "CTAG TBD ESP32 Audio MCU"}],
        "toolsDependencies": []
    }
    rp2040 = {
        "name": "dadamachines CTAG TBD",
        "architecture": "rp2040",
        "version": a.version,
        "category": "Contributed",
        "url": a.base_url + a.rp2040_zip,
        "archiveFileName": a.rp2040_zip,
        "checksum": f"SHA-256:{a.rp2040_sha}",
        "size": to_size(a.rp2040_size, a.size_as_string),
        "boards": [{"name": "CTAG TBD RP2040 UI MCU"}],
        "toolsDependencies": []
    }

    upsert_platform(pkg, esp32)
    upsert_platform(pkg, rp2040)
    sort_platforms(pkg)

    os.makedirs(os.path.dirname(a.index), exist_ok=True)
    with open(a.index, "w", encoding="utf-8") as f:
        json.dump(idx, f, indent=2, ensure_ascii=False)

if __name__ == "__main__":
    main()
