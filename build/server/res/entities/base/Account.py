# -*- coding: utf-8 -*-

# ------------------------------------------------------------------------------
# Section: class Account
# 断开连接与destroy都设置了timeout延后执行，主要是不想entity被频繁的load/save
# ------------------------------------------------------------------------------

import shyloo
class Account(shyloo.Base):
    def __init__(self):
        #self.reservation = 1
        #print("---------------------", self.reservation)
        self.createInNewSpace(1)

    def onGetCell(self):
        self.cell.createAvatarFromSample()
