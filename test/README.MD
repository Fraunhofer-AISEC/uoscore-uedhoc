# Tests

This folder contains tests for uoscore and uedhoc. The tests can be executed on an embedded board, e.g., nrf51dk and/or a Linux host. The tests use Zephyr OS for building. Additional tests for other platforms can be added by adding the following lines in the main function:

```python
    # <board> can be some of the suported by Zephyr OS boards, e.g., nrf51dk_nrf51422
    # <architecture> is a string used only for documentation  
    
    #Example
    run_tests('libuoscore.a', arc('nrf51dk_nrf51422', 'cortex-m0'))
    run_tests('libuedhoc.a', arc('nrf51dk_nrf51422', 'cortex-m0'))
```

### How to run:

* source `zephyr-env.sh`,
* if you want to test on a specific board connect the board to your host and open an serial terminal to observe the test results,
* execute `python3 run.py`

If the tests for a specific platform finish without an error a static library for the corresponding platform is saved in folder `packaged`.