gdb-multiarch build/Robocon2024-Embedded-BoardC.elf \
-ex "target extended-remote localhost:50000" \
-ex "source ares_gdb_plugin.py"