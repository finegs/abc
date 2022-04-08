
def readLine_file(fileName):
    with open(fileName, "r") as file:
        while True:
            line = file.readline()
            if not line:
                break;
            print(line.strip())
#file.close();

def toUpperFile(fromFile, toFile):
    with open(fromFile) as infile, open(toFile, "a") as outfile:
        for line in infile:
            outfile.write("\n"+line.upper())


import sys
import calendar
from urllib.request import urlopen

def fetch_words(url):
    with urlopen(url) as story:
        story_words = []
        for line in story:
            line_words = line.decode('utf-8').split()
            for word in line_words:
                story_words.append(word)
    return story_words


def print_words(story_words):
    for word in story_words:
        print(word)

if __name__ == '__main__':
    url = sys.argv[1]
    words = fetch_words(url)
    print_words(words)
    calendar.setfirstweekday(6)
    calendar.prmonth(1978, 5)

