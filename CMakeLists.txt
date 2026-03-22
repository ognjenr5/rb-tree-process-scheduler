cmake_minimum_required(VERSION 4.0)
project(rb_tree_process_scheduler)

set(CMAKE_CXX_STANDARD 20)

add_executable(rb_tree_process_scheduler
        core/Process.cpp
        core/Process.h
        main.cpp
        core/TreeNode.cpp
        core/TreeNode.h
        core/RedBlackTree.cpp
        core/RedBlackTree.h)
