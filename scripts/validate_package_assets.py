import argparse
import filecmp
import sys
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("package", type=Path)
    args = parser.parse_args()

    staged_assets = Path("build/src/assets")
    packaged_assets = args.package / "src/assets"
    required_assets = (
        "shaders/objectShader/objectShader.vs",
        "shaders/objectShader/objectShader.fs",
        "backpack/backpack.obj",
        "backpack/backpack.mtl",
    )

    missing_required = [
        asset for asset in required_assets if not (staged_assets / asset).is_file()
    ]
    if missing_required:
        print(
            "Build staging is missing required assets: "
            + ", ".join(missing_required),
            file=sys.stderr,
        )
        return 1

    expected = {
        path.relative_to(staged_assets)
        for path in staged_assets.rglob("*")
        if path.is_file()
    }
    packaged = {
        path.relative_to(packaged_assets)
        for path in packaged_assets.rglob("*")
        if path.is_file()
    }
    missing = sorted(expected - packaged)
    unexpected = sorted(packaged - expected)
    mismatched = sorted(
        asset
        for asset in expected & packaged
        if not filecmp.cmp(
            staged_assets / asset,
            packaged_assets / asset,
            shallow=False,
        )
    )

    if missing or unexpected or mismatched:
        if missing:
            print(
                "Package is missing assets: " + ", ".join(map(str, missing)),
                file=sys.stderr,
            )
        if unexpected:
            print(
                "Package contains unstaged assets: "
                + ", ".join(map(str, unexpected)),
                file=sys.stderr,
            )
        if mismatched:
            print(
                "Package asset contents differ: "
                + ", ".join(map(str, mismatched)),
                file=sys.stderr,
            )
        return 1

    print(f"Validated {len(expected)} packaged assets.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
