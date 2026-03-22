# Red-blakc tree process scheduler simulator

This project implements a command-line utility for simulating process scheduling using balanced search trees. It allows for the management and execution of processes based on execution time and waiting constraints.

## Features

* **Balanced Tree Management**: Utilizes a 2-3-4 tree structure to maintain and organize process data efficiently.
* **Process Tracking**: Stores and updates process metadata including execution time, maximum wait time, and current state.
* **Dual Visualization**: Supports outputting the tree structure in both 2-3-4 and Red-Black tree formats.
* **Simulation Engine**: Provides functionality for single-step time increments or full simulation until all processes are completed.
* **Flexible Input**: Supports process insertion via manual console input or batch loading from text files.

## Build and Execution

Compile using g++ (recommended version C++17 or above)

```bash
git clone https://github.com/ognjenr5/rb-tree-process-scheduler.git
cd ./rb-tree-process-scheduler
g++ -o scheduler main.cpp core/Process.cpp core/TreeNode.cpp core/RedBlackTree.cpp -I.
```

To run program:
```bash
./scheduler
```
