#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Twitter DM Static Analysis Tool - Python 包安装脚本

这个脚本配置了 Python 包的构建过程，包括 C++ 扩展模块的编译。
使用 pybind11 和 CMake 来构建 C++ 后端。
"""

import shutil
import sys
from pathlib import Path

from pybind11.setup_helpers import build_ext
from setuptools import setup

# 项目根目录
project_root = Path(__file__).parent


# 读取版本信息
def get_version():
    """从 pyproject.toml 或其他源获取版本号"""
    return "0.1.0"


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
    
    由于项目使用 CMake 构建系统，这里返回空列表，
    实际的扩展构建由 CustomBuildExt 中的 CMake 处理。
    
    Returns:
        list: 空的扩展模块列表
    """
    # 返回空列表，让 CMake 处理实际的构建
    return []


# 自定义构建命令
class CustomBuildExt(build_ext):
    """
    自定义构建扩展命令
    
    这个类扩展了标准的 build_ext 命令，添加了对 CMake 的支持。
    """

    def build_extensions(self):
        """
        构建扩展模块
        
        如果存在 CMakeLists.txt，优先使用 CMake 构建；
        否则使用标准的 setuptools 构建过程。
        """
        cmake_file = project_root / "CMakeLists.txt"

        if cmake_file.exists():
            # 使用 CMake 构建
            self._build_with_cmake()
        else:
            # 使用标准构建
            super().build_extensions()

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
            "*.so",  # 更通用的模式，以防文件名不完全匹配
            "*.pyd",
            "*.dylib",
        ]

        # 确保目标目录存在
        dst_dir = Path(self.build_lib) / "twitter_dm"
        dst_dir.mkdir(parents=True, exist_ok=True)
        
        # 记录是否找到了任何文件
        found_files = False

        for pattern in patterns:
            files = list(build_dir.glob(f"**/{pattern}"))
            for src_file in files:
                # 只复制与twitter_dm相关的文件
                if "twitter_dm" in src_file.name or "twitter-dm" in src_file.name:
                    # 复制到构建目录的twitter_dm子目录
                    dst_file = dst_dir / src_file.name
                    shutil.copy2(src_file, dst_file)
                    print(f"复制扩展模块: {src_file} -> {dst_file}")
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


# 主安装配置
if __name__ == "__main__":
    setup(
        name="twitter-dm-static",
        version=get_version(),
        author="robin",
        author_email="robin528919@gmail.com",
        description="Twitter DM static analysis tool with C++ backend",
        long_description=get_long_description(),
        long_description_content_type="text/markdown",
        url="https://github.com/yourusername/twitter-dm-static",

        # Python 包配置
        packages=["twitter_dm"],  # 包含 twitter_dm Python 包

        # C++ 扩展模块（由 CMake 构建）
        ext_modules=get_extensions(),
        cmdclass={"build_ext": CustomBuildExt},

        # Python 版本要求
        python_requires=">=3.8",

        # 运行时依赖
        install_requires=[
            # 在这里添加运行时依赖
        ],

        # 开发依赖
        extras_require={
            "dev": [
                "pytest>=6.0",
                "pytest-cov",
                "black",
                "flake8",
            ],
        },

        # 分类信息
        classifiers=[
            "Development Status :: 3 - Alpha",
            "Intended Audience :: Developers",
            "License :: OSI Approved :: MIT License",
            "Programming Language :: Python :: 3",
            "Programming Language :: Python :: 3.8",
            "Programming Language :: Python :: 3.9",
            "Programming Language :: Python :: 3.10",
            "Programming Language :: Python :: 3.11",
            "Programming Language :: Python :: 3.12",
            "Programming Language :: C++",
        ],

        # 包含数据文件
        include_package_data=True,
        zip_safe=False,  # C++ 扩展不能压缩
    )
