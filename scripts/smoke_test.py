import argparse
import os
import re
import signal
import subprocess
import sys
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--duration", type=float, default=5)
    parser.add_argument("--cwd", type=Path)
    parser.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    if not args.command:
        parser.error("provide the application command to run")

    print(f"Launching built application: {' '.join(args.command)}", flush=True)
    application_log = Path("logs/application.log")
    application_log.parent.mkdir(parents=True, exist_ok=True)
    command = list(args.command)
    working_directory = args.cwd.resolve() if args.cwd else None
    if os.name == "nt":
        executable = Path(command[0])
        if not executable.is_absolute():
            command[0] = str(((working_directory or Path.cwd()) / executable).resolve())

    try:
        process = subprocess.Popen(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            start_new_session=(os.name != "nt"),
            cwd=working_directory,
        )
    except OSError as error:
        message = (
            f"Command: {' '.join(command)}\n"
            f"Working directory: {working_directory or Path.cwd()}\n"
            f"Failed to start application: {error}\n"
        )
        print(message, file=sys.stderr, end="", flush=True)
        with application_log.open("wb") as log_file:
            log_file.write(message.encode("utf-8"))
        return 1

    timed_out = False
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
        log_file.write(f"Command: {' '.join(command)}\n".encode("utf-8"))
        log_file.write(f"Working directory: {working_directory or Path.cwd()}\n\n".encode("utf-8"))
        if not output:
            log_file.write(b"[Application produced no stdout or stderr during the smoke test.]\n")
        log_file.write(output)
        if output and not output.endswith(b"\n"):
            log_file.write(b"\n")
        status = "timed out as expected" if timed_out else f"exited with status {process.returncode}"
        log_file.write(f"\nSmoke-test result: {status}.\n".encode("utf-8"))
    if output:
        sys.stdout.buffer.write(output)
        if not output.endswith(b"\n"):
            sys.stdout.buffer.write(b"\n")
        sys.stdout.buffer.flush()

    return_code = process.returncode
    shader_error = re.search(
        rb"(?:FILE_NOT_SUCCESFULLY_READ|COMPILATION_FAILED|LINKING_FAILED)",
        output,
    )
    if shader_error:
        print("Application reported a shader load, compile, or link failure.", file=sys.stderr, flush=True)
        return 1
    if timed_out:
        return 0
    print(f"Application exited before the smoke-test interval with status {return_code}.", flush=True)
    return 1


if __name__ == "__main__":
    sys.exit(main())
