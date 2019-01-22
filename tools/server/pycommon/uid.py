# -*- coding: utf-8 -*-

import os
import pwd
#import log

def getuid(name = None):
    return query("uid", name)

def getname(uid = None):
    return query("name", uid)

def query(ret, name = None):
    try:
        name = int(name)
    except:
        pass

    try:
        if type(name) is int:
            user = pwd.getpwuid(name)
        elif type(name) is str:
            user = pwd.getpwnam(name)
        elif name is None:
            user = pwd.getpwuid(os.getuid())
    except:
        print("could't find user[%s] pid " % name)
        return

    if ret == "uid":
        return user.pw_uid
    else:
        return user.pw_name
