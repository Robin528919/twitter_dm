#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Twitter DM Static Analysis Tool - Python 包初始化文件

这个包提供了 Twitter 私信静态分析工具的 Python 接口。
底层使用 C++ 实现高性能的批量私信发送功能。
"""

__version__ = "0.1.0"
__author__ = "robin"
__email__ = "robin528919@gmial.com"
__description__ = "Twitter DM static analysis tool with C++ backend"

# 尝试导入 C++ 扩展模块
try:
    # 导入由 pybind11 生成的 C++ 扩展模块
    from .twitter_dm import *  # noqa: F401, F403
    
    # 标记 C++ 扩展可用
    _cpp_extension_available = True
    
except ImportError as e:
    # 如果 C++ 扩展不可用，提供友好的错误信息
    _cpp_extension_available = False
    _import_error = str(e)
    
    def _raise_import_error(*args, **kwargs):
        """当 C++ 扩展不可用时抛出错误"""
        raise ImportError(
            f"C++ 扩展模块不可用: {_import_error}\n"
            "请确保项目已正确编译。运行 'pip install -e .' 来构建扩展模块。"
        )
    
    # 创建占位符类，避免导入错误
    class DMResult:
        def __init__(self, *args, **kwargs):
            _raise_import_error()
    
    class BatchDMResult:
        def __init__(self, *args, **kwargs):
            _raise_import_error()
    
    class Twitter:
        def __init__(self, *args, **kwargs):
            _raise_import_error()


def get_version():
    """
    获取包版本号
    
    Returns:
        str: 版本号字符串
    """
    return __version__


def is_cpp_extension_available():
    """
    检查 C++ 扩展模块是否可用
    
    Returns:
        bool: 如果 C++ 扩展可用返回 True，否则返回 False
    """
    return _cpp_extension_available


def get_build_info():
    """
    获取构建信息
    
    Returns:
        dict: 包含构建信息的字典
    """
    info = {
        "version": __version__,
        "cpp_extension_available": _cpp_extension_available,
    }
    
    if not _cpp_extension_available:
        info["import_error"] = _import_error
    
    return info


# 导出的公共接口
__all__ = [
    "__version__",
    "get_version",
    "is_cpp_extension_available",
    "get_build_info",
]

# 如果 C++ 扩展可用，添加到导出列表
if _cpp_extension_available:
    __all__.extend([
        "DMResult",
        "BatchDMResult", 
        "Twitter",
    ])