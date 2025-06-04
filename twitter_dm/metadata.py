#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Twitter DM Static Analysis Tool - 元数据文件

这个文件包含了项目的元数据，如版本号、作者信息等。
其他文件应该从这里导入这些信息，而不是重复定义。
"""

# 项目元数据
__name__ = "twitter-dm-static"
__version__ = "0.1.4"
__author__ = "robin"
__email__ = "robin528919@gmail.com"
__description__ = "Twitter DM static analysis tool with C++ backend"
__url__ = "https://github.com/yourusername/twitter-dm-static"
__license__ = "MIT"

# Python 版本要求
__requires_python__ = ">=3.8"

# 分类信息
__classifiers__ = [
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
]

# 项目依赖
__dependencies__ = [
    # 运行时依赖可以在这里添加
    # 例如: "requests>=2.25.0",
    # "numpy>=1.20.0"
]

# 开发依赖
__dev_dependencies__ = [
    "pytest>=6.0",
    "pytest-cov",
    "black",
    "flake8",
]

# 项目 URL
__urls__ = {
    "Homepage": "https://github.com/yourusername/twitter-dm-static",
    "Repository": "https://github.com/yourusername/twitter-dm-static.git",
    "Issues": "https://github.com/yourusername/twitter-dm-static/issues",
}

# 导出的公共接口
__all__ = [
    "__name__",
    "__version__",
    "__author__",
    "__email__",
    "__description__",
    "__url__",
    "__license__",
    "__requires_python__",
    "__classifiers__",
    "__dependencies__",
    "__dev_dependencies__",
    "__urls__",
]
