#!/bin/bash
sysbench fileio prepare
sysbench --file-block-size=4096 --file-test-mode=rndrd --file-extra-flags=direct fileio run
sysbench fileio cleanup