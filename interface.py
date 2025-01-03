import sys
import subprocess
from enum import Enum
from pathlib import Path
import numpy as np
import numpy.typing as npt

# relative to the location where this is called from
DATA_DIR: Path = Path("data")
CPP_EXECUTABLE: Path = Path("bin") / "lerw"


class Norm(Enum):
    LINF = 0
    L1 = 1
    L2 = 2


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

    filename = _format_filename(dimension, distance, number_of_walks, alpha, norm, seed)
    file_path = DATA_DIR / filename

    if recompute:
        file_path.unlink(missing_ok=True)

    if not file_path.exists():
        cmd = [
            Path.cwd() / CPP_EXECUTABLE,
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
    dimension: int,
    distance: float,
    number_of_walks: int,
    alpha: float,
    norm: Norm,
    seed: int = 42,
) -> str:
    return f"walks_dim{dimension}_dist{distance}_n{number_of_walks}_a{alpha}_{norm.name}_rng{seed}.txt"


def test():
    args = {
        "dimension": 2,
        "distance": 5000,
        "number_of_walks": 10,
        "alpha": 0.5,
        "norm": Norm.L2,
        "seed": 2,
    }
    file = Path(DATA_DIR) / _format_filename(**args)
    file.unlink(missing_ok=True)

    walks = get_walk_lengths(**args)
    assert file.exists()
    assert len(walks) == 10
    assert walks[0] == 51
    print("all good")


if __name__ == "__main__":
    test()
