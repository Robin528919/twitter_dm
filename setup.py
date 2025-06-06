#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Twitter DM Static Analysis Tool - Python 包安装脚本

这个脚本配置了 Python 包的构建过程，包括 C++ 扩展模块的编译。
使用 pybind11 和 CMake 来构建 C++ 后端。
"""

import shutil
import sys
import tomli
from pathlib import Path

from pybind11.setup_helpers import build_ext
from setuptools import setup

# 项目根目录
project_root = Path(__file__).parent

# 读取版本信息
def get_version():
    """从 pyproject.toml 获取版本号"""
    with open("pyproject.toml", "rb") as f:
        data = tomli.load(f)
    return data["project"]["version"]

# 读取长描述
def get_long_description():
    """从 README.md 读取长描述"""
    readme_path = project_root / "README.md"
    if readme_path.exists():
        with open(readme_path, "r", encoding="utf-8") as f:
            return f.read()
    return ""

# 定义 C++ 扩展模块
def get_extensions():
    """
    定义 C++ 扩展模块配置
    
    我们声明一个Extension对象，以便setuptools知道我们期望一个名为twitter_dm的扩展模块。
    实际的构建由CustomBuildExt中的CMake处理。
    
    Returns:
        list: 包含一个Extension对象的列表
    """
    from setuptools import Extension
    return [
        Extension(
            "twitter_dm",  # 最终的模块名
            sources=[]     # 源文件由CMake处理，这里为空列表
        )
    ]

# 自定义构建命令
class CustomBuildExt(build_ext):
    """
    自定义构建扩展命令
    
    这个类扩展了标准的 build_ext 命令，添加了对预编译库和 CMake 的支持。
    优先使用预编译的库文件，如果不存在才进行 CMake 构建。
    """

    def build_extensions(self):
        """
        构建扩展模块
        
        优先级：
        1. 检查是否存在预编译的库文件，如果存在直接使用
        2. 如果存在 CMakeLists.txt，使用 CMake 构建
        3. 否则使用标准的 setuptools 构建过程
        """
        # # 首先检查预编译的库文件
        # if self._check_and_use_precompiled():
        #     print("使用预编译的库文件")
        #     return

        cmake_file = project_root / "CMakeLists.txt"
        if cmake_file.exists():
            # 使用 CMake 构建
            print("未找到预编译库文件，开始 CMake 构建")
            self._build_with_cmake()
        else:
            # 使用标准构建
            super().build_extensions()

    def _check_and_use_precompiled(self):
        """
        检查并使用预编译的库文件
        
        Returns:
            bool: 如果找到并成功复制了预编译库文件返回 True，否则返回 False
        """
        # 可能的预编译库文件位置
        precompiled_paths = [
            project_root / "cmake-build-release",
            project_root / "build",
            project_root / "cmake-build-debug",
            project_root / "Release",
            project_root / "Debug",
        ]
        
        # 可能的库文件模式
        patterns = [
            "twitter_dm*.so",     # Linux/macOS 共享库
            "twitter_dm*.pyd",    # Windows Python 扩展
            "twitter_dm*.dylib",  # macOS 动态库
            "libcpr*.dylib",      # CPR 库依赖（包括符号链接）
            "libcurl*.dylib",     # CURL 库依赖
            "libspdlog*.dylib",   # spdlog 库依赖
            "libz*.dylib",        # zlib 库依赖
        ]
        
        # 使用集合来存储唯一的文件名，避免重复复制
        found_files = set()
        
        # 搜索预编译库文件
        for search_path in precompiled_paths:
            if not search_path.exists():
                continue
                
            for pattern in patterns:
                files = list(search_path.glob(f"**/{pattern}"))
                for file_path in files:
                    if file_path.is_file():
                        # 只添加第一个找到的每个唯一文件名
                        if file_path.name not in [f.name for f in found_files]:
                            found_files.add(file_path)
                            print(f"找到预编译库文件: {file_path}")
        
        if not found_files:
            return False
            
        # 确保目标目录存在
        dst_dir = Path(self.build_lib)
        dst_dir.mkdir(parents=True, exist_ok=True)
        
        # 复制找到的库文件
        main_so_file = None
        for src_file in found_files:
            dst_file = dst_dir / src_file.name
            # 检查源文件和目标文件是否相同
            if src_file.resolve() != dst_file.resolve():
                # 如果是符号链接，保持符号链接
                if src_file.is_symlink():
                    # 复制符号链接目标文件
                    target = src_file.readlink()
                    if target.is_absolute():
                        # 绝对路径，直接复制目标文件
                        shutil.copy2(src_file.resolve(), dst_file)
                    else:
                        # 相对路径，先复制目标文件，再创建符号链接
                        target_src = src_file.parent / target
                        if target_src.exists():
                            target_dst = dst_dir / target.name
                            if not target_dst.exists():
                                shutil.copy2(target_src, target_dst)
                            # 创建符号链接
                            if dst_file.exists():
                                dst_file.unlink()
                            dst_file.symlink_to(target.name)
                        else:
                            # 目标文件不存在，直接复制
                            shutil.copy2(src_file, dst_file)
                else:
                    shutil.copy2(src_file, dst_file)
                print(f"复制预编译库文件: {src_file} -> {dst_file}")
                
                # 记录主要的.so文件，用于后续修复依赖路径
                if "twitter_dm" in src_file.name and src_file.name.endswith(".so"):
                    main_so_file = dst_file
            else:
                print(f"跳过复制相同文件: {src_file}")
        
        # 修复主要.so文件的依赖库路径
        if main_so_file and main_so_file.exists():
            self._fix_library_paths(main_so_file)
            
        return True
    
    def _fix_library_paths(self, so_file):
        """
        修复.so文件中的依赖库路径，将@rpath改为@loader_path
        
        Args:
            so_file (Path): 需要修复的.so文件路径
        """
        import subprocess
        
        # 需要修复的依赖库列表
        dependencies = [
            ("@rpath/libcpr.1.dylib", "@loader_path/libcpr.1.dylib"),
            ("@rpath/libcurl.4.dylib", "@loader_path/libcurl.4.dylib"),
            ("@rpath/libspdlog.1.12.dylib", "@loader_path/libspdlog.1.12.dylib"),
        ]
        
        for old_path, new_path in dependencies:
            try:
                subprocess.run([
                    "install_name_tool",
                    "-change",
                    old_path,
                    new_path,
                    str(so_file)
                ], check=True, capture_output=True)
                print(f"修复依赖路径: {old_path} -> {new_path}")
            except subprocess.CalledProcessError as e:
                # 如果依赖不存在，忽略错误
                print(f"跳过不存在的依赖: {old_path}")
            except FileNotFoundError:
                print("警告: install_name_tool 未找到，跳过依赖路径修复")

    def _build_with_cmake(self):
        """
        使用 CMake 构建扩展模块
        
        这个方法调用 CMake 来构建 C++ 扩展，
        然后将生成的库文件复制到正确的位置。
        """
        import subprocess

        # 创建构建目录
        build_dir = project_root / "build"
        build_dir.mkdir(exist_ok=True)

        # CMake 配置
        cmake_args = [
            "-DCMAKE_BUILD_TYPE=Release",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            "-DUSE_LOCAL_PYTHON_ENV=ON",
        ]

        # 运行 CMake 配置
        subprocess.run([
                           "cmake",
                           "-S", str(project_root),
                           "-B", str(build_dir)
                       ] + cmake_args, check=True)

        # 运行 CMake 构建
        subprocess.run([
            "cmake",
            "--build", str(build_dir),
            "--config", "Release"
        ], check=True)

        # 查找生成的库文件并复制到正确位置
        self._copy_built_extensions(build_dir)

        # 新增: 如果是 Linux 系统，在复制完文件后修复 RPATH
        if sys.platform.startswith('linux'):
            self._fix_rpath_linux(Path(self.build_lib)) # self.build_lib 是复制的目标目录

    def _copy_built_extensions(self, build_dir):
        """
        复制构建好的扩展模块到安装目录
        
        Args:
            build_dir (Path): CMake 构建目录
        """
        # 查找生成的扩展文件
        patterns = [
            "twitter_dm*.so",  # Linux/macOS
            "twitter_dm*.pyd",  # Windows
            "twitter_dm*.dylib",  # macOS 动态库
            "*.dll",  # Windows动态库
            "*.so",  # 更通用的模式，以防文件名不完全匹配
            "*.pyd",
            "*.dylib",
        ]

        # 确保目标目录存在 - 直接使用根目录，不再使用twitter_dm子目录
        dst_dir = Path(self.build_lib)
        dst_dir.mkdir(parents=True, exist_ok=True)
        
        # 记录是否找到了任何文件
        found_files = False

        for pattern in patterns:
            files = list(build_dir.glob(f"**/{pattern}"))
            for src_file in files:
                # 复制扩展模块和依赖库
                if ("twitter_dm" in src_file.name or 
                    "twitter-dm" in src_file.name or
                    "libcpr" in src_file.name or
                    "libcurl" in src_file.name or
                    "libz" in src_file.name):
                    # 复制到构建目录的根目录
                    dst_file = dst_dir / src_file.name
                    # 检查源文件和目标文件是否相同，避免原地复制错误
                    if src_file.resolve() != dst_file.resolve():
                        shutil.copy2(src_file, dst_file)
                        print(f"复制文件: {src_file} -> {dst_file}")
                    else:
                        print(f"跳过复制相同文件: {src_file}")
                    found_files = True
        
        # 如果没有找到任何文件，打印警告
        if not found_files:
            print("警告: 未找到任何扩展模块文件！")
            print(f"搜索目录: {build_dir}")
            print("尝试列出构建目录中的所有文件:")
            all_files = list(build_dir.glob("**/*"))
            for file in all_files:
                if file.is_file():
                    print(f"  {file}")

    def _fix_rpath_linux(self, install_dir: Path):
        """
        在 Linux 上修复 .so 文件的 RPATH，将其设置为 $ORIGIN。
        这有助于主扩展模块在其安装目录中找到其依赖的共享库。

        Args:
            install_dir (Path): 包含已安装 .so 文件的目录 (通常是 self.build_lib)。
        """
        import subprocess # 方法内导入 subprocess
        import glob     # 方法内导入 glob

        print(f"尝试在 Linux 上修复 RPATH，目标目录: {install_dir}")
        
        # 查找主扩展模块，通常命名为 <package_name>*.so
        # Extension(name="twitter_dm", ...) -> twitter_dm*.so
        main_module_candidates = list(install_dir.glob("twitter_dm*.so"))

        if not main_module_candidates:
            print(f"警告: 在 {install_dir} 中未找到 twitter_dm*.so 主模块文件，跳过 RPATH 设置。")
            return

        for so_file in main_module_candidates:
            if so_file.is_file():
                print(f"为 {so_file.name} 设置 RPATH 为 $ORIGIN")
                try:
                    # 检查 patchelf 是否可用
                    patchelf_check = subprocess.run(["patchelf", "--version"], check=False, capture_output=True, text=True)
                    if patchelf_check.returncode != 0:
                        print(f"警告: patchelf 命令执行失败 (返回码: {patchelf_check.returncode}) 或未找到。无法为 {so_file.name} 设置 RPATH.")
                        if patchelf_check.stderr:
                            print(f"  patchelf 错误信息: {patchelf_check.stderr.strip()}")
                        elif patchelf_check.stdout:
                            print(f"  patchelf 输出信息: {patchelf_check.stdout.strip()}")
                        continue # 跳过此文件的 RPATH 设置
                    
                    # 设置 RPATH 为 $ORIGIN，这样它会在自己的目录中查找依赖
                    # 注意：$ORIGIN 需要被 shell 解释，或者 patchelf 直接支持它
                    # 在 subprocess.run 中，如果 shell=False (默认)，特殊字符如 $ 不会被 shell 扩展
                    # patchelf 工具自身能够理解 '$ORIGIN'
                    result = subprocess.run(
                        ["patchelf", "--set-rpath", "$ORIGIN", str(so_file)],
                        check=True, # 如果 patchelf 失败则抛出异常
                        capture_output=True,
                        text=True
                    )
                    print(f"成功为 {so_file.name} 设置 RPATH 为 $ORIGIN")
                    if result.stdout:
                        print(f"  patchelf 输出: {result.stdout.strip()}")
                    if result.stderr: # 通常成功时 stderr 为空
                        print(f"  patchelf 警告/错误: {result.stderr.strip()}")

                except FileNotFoundError:
                    print(f"警告: patchelf 命令未找到。无法为 {so_file.name} 设置 RPATH。请确保 patchelf 已安装并位于 PATH 中。")
                except subprocess.CalledProcessError as e:
                    print(f"警告: 使用 patchelf 为 {so_file.name} 设置 RPATH 失败。")
                    print(f"  命令: {' '.join(e.cmd)}")
                    print(f"  返回码: {e.returncode}")
                    if e.stdout:
                        print(f"  输出: {e.stdout.strip()}")
                    if e.stderr:
                        print(f"  错误: {e.stderr.strip()}")
            else:
                print(f"警告: 找到的路径 {so_file} 不是文件，跳过 RPATH 设置。")

# 从pyproject.toml读取项目信息
def get_project_info():
    """从pyproject.toml读取项目信息"""
    with open("pyproject.toml", "rb") as f:
        data = tomli.load(f)
    project = data.get("project", {})
    return project

# 主安装配置
if __name__ == "__main__":
    # 读取项目信息
    project_info = get_project_info()
    
    setup(
        name=project_info.get("name", "twitter-dm-static"),
        version=get_version(),
        author=project_info.get("authors", [{}])[0].get("name", ""),
        author_email=project_info.get("authors", [{}])[0].get("email", ""),
        description=project_info.get("description", ""),
        long_description=get_long_description(),
        long_description_content_type="text/markdown",
        url=project_info.get("urls", {}).get("Homepage", ""),
        license=project_info.get("license", {}).get("text", ""),

        # 不再包含Python包
        # packages=["twitter_dm"],

        # C++ 扩展模块（由 CMake 构建）
        ext_modules=get_extensions(),
        cmdclass={"build_ext": CustomBuildExt},

        # Python 版本要求
        python_requires=project_info.get("requires-python", ">=3.8"),

        # 运行时依赖
        install_requires=project_info.get("dependencies", []),

        # 开发依赖
        extras_require=project_info.get("optional-dependencies", {}),

        # 分类信息
        classifiers=project_info.get("classifiers", []),

        # 包含数据文件
        include_package_data=True,
        zip_safe=False,  # C++ 扩展不能压缩
    )
