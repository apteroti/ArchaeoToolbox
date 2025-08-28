# ArchaeoToolbox

**ArchaeoToolbox** is a cross-platform application for performing **3D geometric morphometrics**, tailored for archaeozoology, palaeontology, and related fields. It provides innovative tools for landmark digitisation, sliding curve and surface semilandmarks and PCA —all in an interactive Qt-based interface.

---

## 🔧 Requirements

If you wish to build from source, ensure the following libraries are available on your system:

- [Qt 5.15.2](https://doc.qt.io/qt-5.15/index.html)
- [VTK 8.2+](https://vtk.org/)
- [Eigen 3](https://eigen.tuxfamily.org/)
- [VCGLib 2025.07](http://vcglib.net)
- CMake 3.16 or newer
- C++17 compatible compiler

---

## 🖥️ Precompiled Binaries

### Windows Users 🪟 Microsoft
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
- ArchaeoToolbox/
- │
- ├── src/                  # Source files
- ├── include/              # Header files and external header-only libraries
- ├── Resources/            # Qt resource files (e.g., icons)
- ├── Binaries/             # Precompiled installers (Windows & Linux)
- ├── CMakeLists.txt        # Build configuration
- ├── LICENSE               # The licence file
- ├── README.md             # This file

## 📃 License
This software is licensed under the GPLv3, which means you are free to use, modify, and distribute it — but any derivative work must also be released under the same license. This ensures that the software and any improvements remain free and open for everyone. For more information, please read the licence file.
