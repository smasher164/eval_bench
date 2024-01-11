# Gotten from Eli Bendersky's code for his blog, licensed with Unlicense.
# Run this with Python 3 to fill up a file with random integer bytes that
# represent our interpreter's opcodes.
import random

with open('zz.bin', 'wb') as f:
    for i in range(120000):
        f.write(random.randint(1, 6).to_bytes(1, 'little'))
