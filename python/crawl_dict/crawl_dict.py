#encoding:utf-8
import urllib
import re
import threading

class Spider:
    def __init__(self):
        self.dic = {}
        self.queue = ['你好']
        self.open = set(self.queue)
        self.count = 0
        self.lock = threading.Lock()
        self.thread_num = 10
        self.words_total = 1000
        

    def start(self):
        threads = []
        for i in range(self.thread_num):
            thread = threading.Thread(target = self.go_on, name = str(i))
            thread.setDaemon(True)
            thread.start()
            threads.append(thread)
        #wait all
        for t in threads:
            t.join()

    def go_on(self):
        while len(self.queue) > 0 and self.count <self.words_total:
            word = ''
            with self.lock:
                word = self.queue.pop(0)
                self.open.remove(word)
            self.crawl(word)
           

    def crawl(self, keyword):
        params = urllib.urlencode({'q': keyword})
        data = ''
        #when except occurs, continue to get pinyin
        while True:
            try:
                f = urllib.urlopen("http://www.zdic.net/sousuo/", params)
                data = f.read()
                break
            except:
                print 'except occur, continue...'
                continue
        m1 = re.search('spc\(.+\)', data)
        if m1 is not None: 
            letters = m1.group(0)[5:-2]
            print self.count, keyword.decode('utf-8'), letters, '(', threading.currentThread().getName(), ')'
            #***add in dict***
            with self.lock:
                #print 'add'
                self.dic[keyword] = letters
                self.count += 1
            #crawl link
            m2 = re.search('<p><strong>相关词语：</strong></p>', data)
            start = m2.end()
            m3 = re.search('<div class="annu_div">', data)
            end = m3.start()
            data = data[start : end].decode('utf-8')
            p = re.compile(u'[\u4e00-\u9fa5]+')
            alls = p.finditer(data)
            words = [i.group().encode('utf-8') for i in alls]
            for word in words:
                #print word.decode('utf-8')
                #***judge if in***
                if (word not in self.dic) and (word not in self.open):
                    with self.lock:
                        self.queue.append(word)
                        self.open.add(word)


if __name__ == '__main__':
    print 'robin1001 here'
    #Spider().start()
    
        
