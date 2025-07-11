from wordcloud import WordCloud, STOPWORDS
from matplotlib import font_manager, rc
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image
import platform


path = "/usr/local/share/fonts/malgun.ttf"
wine_mask = np.array(Image.open("/home/user01/dev/git/abc/wordcloud/wine_mask.png"))
font_name = font_manager.FontProperties(fname=path).get_name()

# text = open("/home/user01/dev/git/abc/wordcloud/dict.csv").read()
text = open("/home/user01/dev/git/abc/wordcloud/words.txt").read()

stopwords = set(STOPWORDS)
stopwords.add("unto")
stopwords.add("ye")


wc = WordCloud(
    background_color="white", max_words=2000, mask=wine_mask, stopwords=stopwords
)
wc = wc.generate(text)
wc.words_

rc("font", family=font_name)
plt.figure(figsize=(8, 8))
plt.imshow(wine_mask, interpolation="bilinear")
plt.axis("off")
plt.show()

'''
from wordcloud import WordCloud
import numpy as np
from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

from wordcloud import get_single_color_func


class GroupedColorFunc(object):
    def __init__(self, color_to_words, default_color):
        self.color_func_to_words = [
            (get_single_color_func(color), set(words))
            for (color, words) in color_to_words.items()
        ]

        self.default_color_func = get_single_color_func(default_color)

    def get_color_func(self, word):
        """Returns a single_color_func associated with the word"""
        try:
            color_func = next(
                color_func
                for (color_func, words) in self.color_func_to_words
                if word in words
            )
        except StopIteration:
            color_func = self.default_color_func

        return color_func

    def __call__(self, word, **kwargs):
        return self.get_color_func(word)(word, **kwargs)


class SimpleGroupedColorFunc(object):
    def __init__(self, color_to_words, default_color):
        self.word_to_color = {
            word: color for (color, words) in color_to_words.items() for word in words
        }

        self.default_color = default_color

    def __call__(self, word, **kwargs):
        return self.word_to_color.get(word, self.default_color)


# alice_mask = np.array(Image.open("/home/user01/dev/git/abc/wordcloud/alice_mask.png"))
alice_mask = np.array(Image.open("/home/user01/dev/git/abc/wordcloud/wine_mask.png"))

# font_path = "/mnt/c/Windows/fonts/h2gtre.ttf"
font_path = "/mnt/c/Windows/fonts/malgun.ttf"
text = open("/home/user01/dev/git/abc/wordcloud/../dict.csv").read()

wordcloud = WordCloud(
    font_path=font_path,
    width=800,
    height=800,
    background_color="",
    mask=alice_mask,
)

color_to_words = wordcloud.generate(text)
default_color = wordcloud.to_array()

grouped_color_func = GroupedColorFunc(color_to_words, default_color)
wordcloud.recolor(color_func=grouped_color_func)
plt.figure(figsize=(10, 10))
plt.imshow(wordcloud, interpolation="bilinear")
plt.axis("off")
plt.show()

simple_color_func = SimpleGroupedColorFunc(color_to_words, default_color)
wordcloud.recolor(color_func=simple_color_func)
plt.figure(figsize=(10, 10))
plt.imshow(wordcloud, interpolation="bilinear")
plt.axis("off")
plt.show()

'''
