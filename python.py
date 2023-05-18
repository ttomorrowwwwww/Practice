class Information:
    Infocount = 0
    def __init__(self, name, grade):
        self.name = name
        self.grade = grade
        Information.Infocount += 1
    def printcount(self):
        print(f'total students %d'%Information.Infocount)

    def printinfo(self):
        print('Name:', self.name,',grade:', self.grade)
    def printinfo1(self):
        print('Name:', self.name,',grade:', self.grade, ',age:', self.age)

    def __del__(self):
        classname = self.__class__.__name__
        print(f'{classname}销毁')

inf1 = Information('Anna', 5)
inf1.age = 11 #增加属性
#print(getattr(inf1, 'age'))
inf2 = Information('Allen', 9)
setattr(inf2, 'age', 15) #增加属性
#print(getattr(inf2, 'age')) #返回属性值

delattr(inf1, 'age') #删除属性值

#print(f'total students %d'%Information.Infocount)
inf1.printinfo()
inf2.printinfo1()
#print('Information.__doc__:', Information.__doc__)
#print('Information.__name__:', Information.__name__)
#print('Information.__module__:', Information.__module__)
del inf1
del inf2
