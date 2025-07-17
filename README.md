# ArchaeoToolbox

**ArchaeoToolbox** is a cross-platform application for performing **3D geometric morphometrics**, tailored for archaeozoology, palaeontology, and related fields. It provides tools for landmark digitisation, sliding semilandmarks, PCA, strain analysis, and modern methods like Laplace-Beltrami operators—all in an interactive Qt-based interface.

---

## 🔧 Requirements

If you wish to build from source, ensure the following libraries are available on your system:

- [Qt 5.15.2](https://doc.qt.io/qt-5.15/index.html) (Widgets, GUI, Help, Charts, Test modules)
- [VTK 8.2+](https://vtk.org/)
- [CPD library (Coherent Point Drift)](https://github.com/gadomski/cpd)
- [OpenCASCADE](https://dev.opencascade.org/)
- [Eigen3](https://eigen.tuxfamily.org/)
- CMake 3.16 or newer
- C++17 compatible compiler

---

## 🖥️ Precompiled Binaries

### Windows Users
- Go to the `Binaries/` folder in this repository.
- Download the `.exe` installer and run it.
- The application will install like any regular Windows software.

### Linux Users 🐧
- An **AppImage** is provided in the `Binaries/` folder.
- Download the `.AppImage` file.
- Right-click → **Properties** → **Permissions** → Check “Allow executing file as program”.
- Then double-click to run.

---

## 📁 Folder Structure
ArchaeoToolbox/
│
├── src/                  # Source files
├── include/              # Header files
├── Resources/            # Qt resource files (e.g., icons)
├── Binaries/             # Precompiled installers (Windows & Linux)
├── build/                # (Generated) CMake build directory
├── CMakeLists.txt        # Build configuration
├── README.md             # This file

## 📃 License
MIT License — Free to use, modify, and distribute.
