import sys
import subprocess
import random

def deob(s, amt=0):

    if amt==0:
        amt = random.randint(1, 5)



    s2 = ''
    prevslash = False
    for c in s:
        # s = ''.join(chr(((ord(c)+amt-32) % 127) +32) for c in s)

        if prevslash:
            s2 += '\\S_L_A_S_H_\\'+c
            prevslash=False
        elif c=='\\':
            prevslash = True
        else:
            i = ord(c) + amt - 32
            i = i % (127-32)
            i = i + 32
            # print (i)
            s2 += chr(i)
    s = s2
    s = s.replace('\\', '\\\\')
    s = s.replace('"', '\\"')
    s = s.replace('\\\\S_L_A_S_H_\\\\', '\\')
    s = f'UnlockUtil::deob{amt}("'+s+'")'
    return s


if __name__ == "__main__":
    if len(sys.argv) > 1:
        s = sys.argv[1]

        amt = 0
        if len(sys.argv) > 2:
            amt = int(sys.argv[2])

        # pyperclip.copy(s)
        # print (deob(s))
        s = deob(s, amt)
        subprocess.run("pbcopy", universal_newlines=True, input=s)
        print (s)
