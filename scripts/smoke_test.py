import argparse
import os
import signal
import subprocess
import sys
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--duration", type=float, default=5)
    parser.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    if not args.command:
        parser.error("provide the application command to run")

    print(f"Launching built application: {' '.join(args.command)}", flush=True)
    application_log = Path("logs/application.log")
    application_log.parent.mkdir(parents=True, exist_ok=True)
    try:
        process = subprocess.Popen(
            args.command,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            start_new_session=(os.name != "nt"),
        )
    except OSError as error:
        print(f"Failed to start application: {error}", file=sys.stderr, flush=True)
        return 1

    try:
        output, _ = process.communicate(timeout=args.duration)
    except subprocess.TimeoutExpired:
        print(f"Application ran for {args.duration:g} seconds; stopping smoke test.", flush=True)
        if os.name == "nt":
            process.terminate()
        else:
            os.killpg(process.pid, signal.SIGTERM)
        try:
            output, _ = process.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            if os.name == "nt":
                process.kill()
            else:
                os.killpg(process.pid, signal.SIGKILL)
            output, _ = process.communicate()
        timed_out = True
    else:
        timed_out = False

    with application_log.open("wb") as log_file:
        log_file.write(output)
    if output:
        sys.stdout.buffer.write(output)
        if not output.endswith(b"\n"):
            sys.stdout.buffer.write(b"\n")
        sys.stdout.buffer.flush()

    return_code = process.returncode
    if timed_out:
        return 0
    print(f"Application exited before the smoke-test interval with status {return_code}.", flush=True)
    return 1


if __name__ == "__main__":
    sys.exit(main())
