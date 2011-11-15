#!/bin/bash
( echo "const char *fragShader = " && cat gldicom.frag | sed s/\\\`/\"/ | sed s/\\\'/\\\\n\"\\\\/ && echo ";" ) > $1
