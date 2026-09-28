Host audio tests (Linux gcc; no board peripheral simulation):

```
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Itests/audio_stub tests/test_audio.c -o /tmp/nes_test_audio
ASAN_OPTIONS=detect_leaks=0 /tmp/nes_test_audio
```

The test includes the production nes_audio.c with mocked HAL/BSP calls.
Leak checking is disabled because the execution sandbox disallows its process scan;
address and undefined-behavior checks remain enabled. The audio module allocates no heap.
Use the default NES_AUDIO_ENABLE=1, NES_AUDIO_TEST_TONE=0 configuration.
