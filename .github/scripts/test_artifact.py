import argparse
import re
import shlex
import subprocess
import sys
from pathlib import Path
from typing import TextIO


LOG_PATH = Path("test.log")
STARTUP_TIMEOUT_SECONDS = 10
ERROR_PATTERN = re.compile(r"ERROR::|Failed to", re.IGNORECASE)


def record_command(command: list[str], log: TextIO, cwd: Path) -> bool:
    log.write(f"$ {shlex.join(command)}\n")
    log.flush()
    try:
        result = subprocess.run(
            command,
            cwd=cwd,
            stdout=log,
            stderr=subprocess.STDOUT,
            check=False,
        )
    except OSError as error:
        log.write(f"Could not run command: {error}\n")
        return False

    if result.returncode != 0:
        log.write(f"Command exited with status {result.returncode}.\n")
        return False
    return True


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", required=True, type=Path)
    parser.add_argument("--shader-dir", required=True, type=Path)
    parser.add_argument("--runtime-directory", type=Path, default=Path("."))
    parser.add_argument("--runtime-smoke-test", action="store_true")
    args = parser.parse_args()

    workspace = Path.cwd()
    binary = (workspace / args.binary).resolve()
    shader_dir = (workspace / args.shader_dir).resolve()
    runtime_directory = (workspace / args.runtime_directory).resolve()
    passed = True

    with LOG_PATH.open("w", encoding="utf-8") as log:
        if binary.is_file():
            log.write(f"Found build artifact: {binary}\n")
        else:
            log.write(f"Build artifact not found: {binary}\n")
            passed = False

        for shader, stage in (("shader.vs", "vert"), ("shader.fs", "frag")):
            shader_path = shader_dir / shader
            if not shader_path.is_file():
                log.write(f"Shader not found: {shader_path}\n")
                passed = False
                continue
            if not record_command(
                ["glslangValidator", "-S", stage, str(shader_path)],
                log,
                workspace,
            ):
                passed = False

        if args.runtime_smoke_test and binary.is_file():
            log.write(f"Starting application from {runtime_directory}\n")
            log.flush()
            try:
                process = subprocess.Popen(
                    [str(binary)],
                    cwd=runtime_directory,
                    stdout=log,
                    stderr=subprocess.STDOUT,
                )
            except OSError as error:
                log.write(f"Could not start application: {error}\n")
                passed = False
            else:
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
                else:
                    log.write(
                        f"Application exited with status {return_code} "
                        "before the smoke-test timeout.\n"
                    )
                    passed = False

    output = LOG_PATH.read_text(encoding="utf-8", errors="replace")
    if ERROR_PATTERN.search(output):
        passed = False

    if not passed:
        print(f"Build tests failed; see {LOG_PATH}.", file=sys.stderr)
        return 1

    print(f"Build tests passed; see {LOG_PATH} for captured output.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())