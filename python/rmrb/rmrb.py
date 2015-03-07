#encoding:utf-8
#   created by robin1001, 2014-12-21
import urllib
import re
import os
import sys

def read_url(url):
    count = 0
    while True:
        try:
            data = urllib.urlopen(url).read().decode('gbk').encode('utf8')
            break
        except KeybordInterrupt:
            pass
        else:
            print 'try again %d %s' % (count, url)
            count += 1
            continue
    return data

class Spider:
    def __init__(self, begin = 0, end = None):
        self.main_url = 'http://rmrbw.info/'
        self.start_url = 'http://rmrbw.info/simple/'
        self.begin = begin
        self.end = end
    def start(self):
        self.main_page(self.start_url)  

    def main_page(self, url):
        content = read_url(url)
        matches = re.findall('<li><a href="([^"]+)">([^<]+)</a></li>', content)
        if self.end is None:
            self.end = len(matches)
        for cur, m in enumerate(matches):
            if cur < self.begin: continue
            if cur > self.end: break
            new_url = self.main_url + m[0]
            date = m[1]
            print date, new_url 
            self.progress = "current progress %d/%d" % (cur, self.end)
            self.handle_month(new_url, ('%03d_' % cur) + date)

    def handle_month(self, mon_url, date):
        fid = open('./data/'+ date + ".txt", "w")
        content1 = read_url(mon_url)
        #print mon_url
        m = re.search("&nbsp;Pages: \(  (\d+) total \)", content1)
        if m is None:
            return
        pages = int(m.group(1)) #get pages
        #handle all the pages
        for i in range(1, pages+1):
            url = mon_url[:-5] + '_' + str(i) + '.html'
            if i < 2: 
                content = content1
            else:
                content = read_url(url)
            matches = re.findall('<li><a href="([^"]+)">([^<]+)</a> &nbsp; <span class="smalltxt">[^<]*</span></li>', content)
            #print len(matches)
            for m in matches:
                new_url = self.main_url + m[0] 
                title = m[1]
                print self.progress, "write page\t", title, new_url
                sys.stdout.flush()
                self.write_page(new_url, title, fid)
                #break
            #break
        fid.close()            
    def write_page(self, url, title, fid):
        data = read_url(url)
        date_m = re.search('<td align="right" class="smalltxt">([^<]+)</td>', data)
        content_m = re.search('<td colspan="2" class="tpc_content">[^<]+<script[^<]+</script><br>([^/]+)/td>', data)
        #failed to match
        if (date_m == None) or (content_m == None):
            return
        date = date_m.group(1)
        content = content_m.group(1)
        content = re.sub('<br>', '\n', content[4:-1])#delete \r\n\r\n and > 
        #author = re.search('<td><b>([^<]+)</b></td>', data).group(1)
        #print date, content
        fid.write('<article>\n')
        fid.write('<title>')
        fid.write(title)
        fid.write('</title>\n')
        #fid.write('<author>')
        #fid.write(author)
        #fid.write('</author>\n')
        fid.write('<url>')
        fid.write(url)
        fid.write('</url>\n')
        fid.write('<date>')
        fid.write(date)
        fid.write('</date>\n')
        fid.write('<content>\n')
        fid.write(content)
        fid.write('\n</content>\n')
        fid.write('</article>\n')
        fid.write('\n')


if __name__ == '__main__':
    print '''Usage: rmrb.py [start_pos] [end_pos] 
            it is convenient to use start_pos, end _pos when you want to use multiprocess to run accomplish this work''' 
    length = len(sys.argv)
    #print sys.argv, length
    if length== 1:
        Spider().start()
    elif length == 2:
        Spider(int(sys.argv[1])).start()
    else:
        Spider(int(sys.argv[1]), int(sys.argv[2])).start()
    #Spider().start()
