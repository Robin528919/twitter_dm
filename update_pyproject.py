#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
更新 pyproject.toml 文件

这个脚本从 twitter_dm/metadata.py 读取元数据，
然后更新 pyproject.toml 文件中的相应字段。
"""

import importlib.util
import sys
import tomli
import tomli_w
from pathlib import Path

# 项目根目录
project_root = Path(__file__).parent

# 从metadata.py导入元数据
def import_metadata():
    """从 metadata.py 导入元数据"""
    metadata_path = project_root / "twitter_dm" / "metadata.py"
    spec = importlib.util.spec_from_file_location("metadata", metadata_path)
    metadata = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(metadata)
    return metadata

def update_pyproject():
    """更新 pyproject.toml 文件"""
    # 导入元数据
    metadata = import_metadata()
    
    # 读取现有的 pyproject.toml 文件
    pyproject_path = project_root / "pyproject.toml"
    
    try:
        with open(pyproject_path, "rb") as f:
            pyproject = tomli.load(f)
    except FileNotFoundError:
        print(f"错误: 找不到 {pyproject_path} 文件")
        return 1
    except Exception as e:
        print(f"错误: 读取 {pyproject_path} 文件时出错: {e}")
        return 1
    
    # 更新项目元数据
    if "project" not in pyproject:
        pyproject["project"] = {}
    
    pyproject["project"]["name"] = metadata.__name__
    pyproject["project"]["version"] = metadata.__version__
    pyproject["project"]["description"] = metadata.__description__
    pyproject["project"]["requires-python"] = metadata.__requires_python__
    
    # 更新作者信息
    pyproject["project"]["authors"] = [
        {"name": metadata.__author__, "email": metadata.__email__}
    ]
    
    # 更新分类信息
    pyproject["project"]["classifiers"] = metadata.__classifiers__
    
    # 更新依赖
    pyproject["project"]["dependencies"] = metadata.__dependencies__
    
    # 更新可选依赖
    if "optional-dependencies" not in pyproject["project"]:
        pyproject["project"]["optional-dependencies"] = {}
    
    pyproject["project"]["optional-dependencies"]["dev"] = metadata.__dev_dependencies__
    
    # 更新项目 URL
    if "urls" not in pyproject["project"]:
        pyproject["project"]["urls"] = {}
    
    for key, value in metadata.__urls__.items():
        pyproject["project"]["urls"][key] = value
    
    # 写入更新后的 pyproject.toml 文件
    try:
        with open(pyproject_path, "wb") as f:
            tomli_w.dump(pyproject, f)
        print(f"成功更新 {pyproject_path} 文件")
    except Exception as e:
        print(f"错误: 写入 {pyproject_path} 文件时出错: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    # 检查是否安装了 tomli 和 tomli_w
    
    sys.exit(update_pyproject())
