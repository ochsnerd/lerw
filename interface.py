import sys
import subprocess
from enum import Enum
from pathlib import Path
import numpy as np
import numpy.typing as npt

# relative to the location where this is called from
DATA_DIR: str = "data"
CPP_EXECUTABLE: str = "lerw"


class Norm(Enum):
    LINFTY = 0
    L1 = 1
    L2 = 2


def get_walk_lengths(
    dimension: int,
    distance: float,
    number_of_walks: int,
    alpha: float,
    norm: Norm,
) -> npt.NDArray[np.int64]:
    """Get walk lengths from existing file or generate new data using C++ executable.

    Arguments match output of "<cpp_exe> --help".
    """
    data_path = Path(DATA_DIR)
    data_path.mkdir(exist_ok=True)

    filename = f"walks_dim{dimension}_dist{distance}_n{number_of_walks}_a{alpha}_{norm.name}.txt"

    file_path = data_path / filename

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
