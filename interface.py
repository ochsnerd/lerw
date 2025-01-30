import sys
import subprocess
from enum import Enum
from pathlib import Path
import numpy as np
import numpy.typing as npt

# relative to the location where this is called from
DATA_DIR: Path = Path("data")
CPP_LENGTHS_EXE: Path = Path("bin") / "lerw_length"
CPP_POINTS_EXE: Path = Path("bin") / "lerw_points"


class Norm(Enum):
    LINF = 0
    L1 = 1
    L2 = 2


def get_walk(
    number_of_steps: int,
    alpha: float,
    norm: Norm,
    seed: int = 3,
    recompute: bool = False,
) -> npt.NDArray[np.int64]:
    """Get Points of a single walk.

    Arguments match output of "<cpp_exe> --help".
    """
    DATA_DIR.mkdir(exist_ok=True)
    dimension = 2
    distance = 0.0  # ignored by cpp

    filename = _format_filename(
        "walk", dimension, distance, number_of_steps, alpha, norm, seed
    )
    file_path = DATA_DIR / filename

    if recompute:
        file_path.unlink(missing_ok=True)

    if not file_path.exists():
        cmd = [
            Path.cwd() / CPP_POINTS_EXE,
            "--dimension",
            dimension,
            "--distance",
            distance,
            "--number_of_walks",
            number_of_steps,
            "--alpha",
            alpha,
            "--norm",
            norm.name,
            "--output",
            file_path,
            "--seed",
            seed,
        ]

        result = subprocess.run(
            list(map(str, cmd)),
            capture_output=True,
            text=True,
            check=False,
        )

        # Check for any output, which indicates an error
        if result.stdout or result.stderr or result.returncode != 0:
            print(
                f"Failed to call C++:\n{result.stderr}\n\n{result.stdout}",
                file=sys.stderr,
            )
            raise subprocess.CalledProcessError(
                returncode=result.returncode or 1,
                cmd=cmd,
                output=result.stdout,
                stderr=result.stderr,
            )

    return np.genfromtxt(file_path, dtype=np.int64, comments="#", delimiter=",")


def get_walk_lengths(
    dimension: int,
    distance: float,
    number_of_walks: int,
    alpha: float,
    norm: Norm,
    seed: int = 3,
    recompute: bool = False,
) -> npt.NDArray[np.int64]:
    """Get walk lengths from existing file or generate new data using C++ executable.

    Arguments match output of "<cpp_exe> --help".
    """
    DATA_DIR.mkdir(exist_ok=True)

    filename = _format_filename(
        "walks", dimension, distance, number_of_walks, alpha, norm, seed
    )
    file_path = DATA_DIR / filename

    if recompute:
        file_path.unlink(missing_ok=True)

    if not file_path.exists():
        cmd = [
            Path.cwd() / CPP_LENGTHS_EXE,
            "--dimension",
            dimension,
            "--distance",
            distance,
            "--number_of_walks",
            number_of_walks,
            "--alpha",
            alpha,
            "--norm",
            norm.name,
            "--output",
            file_path,
            "--seed",
            seed,
        ]

        result = subprocess.run(
            list(map(str, cmd)),
            capture_output=True,
            text=True,
            check=False,
        )

        # Check for any output, which indicates an error
        if result.stdout or result.stderr or result.returncode != 0:
            print(
                f"Failed to call C++:\n{result.stderr}\n\n{result.stdout}",
                file=sys.stderr,
            )
            raise subprocess.CalledProcessError(
                returncode=result.returncode or 1,
                cmd=cmd,
                output=result.stdout,
                stderr=result.stderr,
            )

    return np.genfromtxt(file_path, dtype=np.int64, comments="#", delimiter="\n")


def _format_filename(
    prefix: str,
    dimension: int,
    distance: float,
    number_of_walks: int,
    alpha: float,
    norm: Norm,
    seed: int = 42,
) -> str:
    return f"{prefix}_dim{dimension}_dist{distance}_n{number_of_walks}_a{alpha}_{norm.name}_rng{seed}.txt"


def test():
    num_steps = 10
    args = {
        "alpha": 0.5,
        "norm": Norm.L2,
        "seed": 2,
    }
    file = Path(DATA_DIR) / _format_filename("walk", 2, 0.0, num_steps, **args)
    file.unlink(missing_ok=True)

    walk = get_walk(num_steps, **args)
    assert file.exists()
    assert walk.shape == (11, 2)
    assert all(walk[10, :] == [16, -1077])

    args = {
        "dimension": 2,
        "distance": 5000,
        "number_of_walks": 10,
        "alpha": 0.5,
        "norm": Norm.L2,
        "seed": 2,
    }
    file = Path(DATA_DIR) / _format_filename("walks", **args)
    file.unlink(missing_ok=True)

    walks = get_walk_lengths(**args)
    assert file.exists()
    assert len(walks) == 10
    assert walks[0] == 51
    print("all good")


if __name__ == "__main__":
    test()
