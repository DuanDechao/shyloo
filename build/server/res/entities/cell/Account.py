# -*- coding: utf-8 -*-

# ------------------------------------------------------------------------------
# Section: class Account
# 断开连接与destroy都设置了timeout延后执行，主要是不想entity被频繁的load/save
# ------------------------------------------------------------------------------

import shyloo
class Account(shyloo.Entity):
    def __init__(self):
        print("check attr value:", self.marketing_account_reward)
        pass
        #self.reservation = 1
        #print("---------------------", self.reservation)
        #self.createCellEntity(self.cell)
        #self.cell.createAvatarFromSample()

    def createAvatarFromSample(self):
        print("+_+++++++++++++++++++ I LOVE FANCY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
