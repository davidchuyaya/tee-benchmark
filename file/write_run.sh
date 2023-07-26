#!/bin/bash
sysbench fileio prepare
sysbench --file-block-size=4096 --file-test-mode=rndwr --file-fsync-all=on fileio run
sysbench fileio cleanup