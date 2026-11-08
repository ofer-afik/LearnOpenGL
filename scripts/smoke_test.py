import argparse
import os
import signal
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--duration", type=float, default=5)
    parser.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    if not args.command:
        parser.error("provide the application command to run")

    print(f"Launching built application: {' '.join(args.command)}", flush=True)
    try:
        process = subprocess.Popen(
            args.command,
            start_new_session=(os.name != "nt"),
        )
    except OSError as error:
        print(f"Failed to start application: {error}", file=sys.stderr, flush=True)
        return 1

    try:
        return_code = process.wait(timeout=args.duration)
    except subprocess.TimeoutExpired:
        print(f"Application ran for {args.duration:g} seconds; stopping smoke test.", flush=True)
        if os.name == "nt":
            process.terminate()
        else:
            os.killpg(process.pid, signal.SIGTERM)
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            if os.name == "nt":
                process.kill()
            else:
                os.killpg(process.pid, signal.SIGKILL)
            process.wait()
        return 0

    print(f"Application exited before the smoke-test interval with status {return_code}.", flush=True)
    return 1


if __name__ == "__main__":
    sys.exit(main())
