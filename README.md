# MEK---Micro-Event-Driven-Kernel

**MEK (Micro Event-Driven Kernel)** is a lightweight **event-driven kernel** designed to manage tasks efficiently using the **Active Object (AO)** model, **priority-based preemption**, and **Hierarchical State Machines (HSM)**.  
Each task owns its own **message queue** and **state machine**; **events** are posted and handled **asynchronously**, and are automatically removed from the queue after processing by the kernel.

### Key Concepts:
- Active Object and priority preemption  
- Per-task message queues  
- Automatic message handling and removal  
- Event-driven architecture
- Hierarchical State Machines

To fully understand this model, learners should have basic knowledge of data structures such as linked lists, queues, memory pools, and object-oriented programming (OOP).

### Features:
- AO Task
- Memory pool management  
- Message posting and handling mechanisms  
- Timer list support  
- Publish/Subscibe pattern.

## References:

| Topic | Link |
|------|------|
| Active Object Model | https://www.state-machine.com/doc/AN_Active_Objects_for_Embedded.pdf |
| Super Simple Tasker | https://github.com/QuantumLeaps/Super-Simple-Tasker |
| QP/C                | https://github.com/QuantumLeaps/qpc |
