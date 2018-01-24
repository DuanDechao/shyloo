# -*- coding: utf-8 -*-

# ------------------------------------------------------------------------------
# Section: class Account
# 断开连接与destroy都设置了timeout延后执行，主要是不想entity被频繁的load/save
# ------------------------------------------------------------------------------

import shyloo
class LoginProxy(shyloo.Proxy):
    def __init__(self):
        pass
        #self.reservation = 1
        print("I AM LoginProxy---------------------")

    def onEntityEnabled(self):
        shyloo.createBaseLocallyFromDB("Account", 122342343, lambda entity, databaseID, wasActive: self.onLoadAccountFromDB(entity, databaseID, wasActive))

    def onLoadAccountFromDB(self, entity, databaseID, wasActive):
        print("onLoadAccountFromDB callback........................")
