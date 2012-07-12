#!/usr/bin/env python
import os

def main():
    with open("/home/rz/Trace/WebSearch2.spc") as f, open("raid_4", "r+") as g:
        for line in f:
            num, offsize, length, op, sec = line.split(',')
            n = int(num)
            if n > 2: continue
            if n == 2: n = 6
            if n == 1: n = 3
            o = int(offsize)
            if o > 22000000:
                n += 2
                o -= 22000000
            elif o > 11000000 and o < 22000000:
                n += 1
                o -= 11000000
            num = str(n)
            offsize = str(o)
            deal = (",").join([num, offsize, length, op, sec])
            g.write(deal)

def ignore():
    pass

if __name__ == '__main__':
    main()
