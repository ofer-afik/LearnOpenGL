import re
import subprocess
import sys
from pathlib import Path


LOG_PATH = Path("smoke-test.log")
STARTUP_TIMEOUT_SECONDS = 10
ERROR_PATTERN = re.compile(r"ERROR::|Failed to", re.IGNORECASE)


def main() -> int:
    if len(sys.argv) != 2:
        print("Usage: smoke_test.py <executable>", file=sys.stderr)
        return 2

    with LOG_PATH.open("w", encoding="utf-8") as log:
        log.write(f"Starting {sys.argv[1]}\n")
        log.flush()
        process = subprocess.Popen(
            [sys.argv[1]],
            stdout=log,
            stderr=subprocess.STDOUT,
        )

        try:
            return_code = process.wait(timeout=STARTUP_TIMEOUT_SECONDS)
        except subprocess.TimeoutExpired:
            process.terminate()
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
            log.write(
                f"Application remained running for "
                f"{STARTUP_TIMEOUT_SECONDS} seconds; launch passed.\n"
            )
            return_code = None

    output = LOG_PATH.read_text(encoding="utf-8", errors="replace")
    errors = ERROR_PATTERN.findall(output)
    if errors:
        print("Application logged startup errors; see smoke-test.log.", file=sys.stderr)
        return 1
    if return_code is not None:
        print(
            f"Application exited after {return_code} before the smoke-test timeout; "
            "see smoke-test.log.",
            file=sys.stderr,
        )
        return 1

    print(f"Application launch passed; see {LOG_PATH} for captured output.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
