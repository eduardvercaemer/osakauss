#!/usr/bin/env bash
# begin a debugging session in tmux
qemu_opts='-soundhw pcspk -m 4096'
kernel='./build/kernel/kernel_stripped'
gdb_script='./scripts/qemu.dbg'

tmux new -s dbg -d
tmux rename-window -t dbg qemu
tmux send-keys -t dbg "qemu-system-i386 ${qemu_opts} -display none -serial stdio -kernel ${kernel} -s -S"  C-m

tmux split-window -h -t dbg
tmux rename-window -t dbg gdb
tmux send-keys -t dbg "sleep 1 && gdb -x ${gdb_script}" C-m

tmux attach -t dbg
