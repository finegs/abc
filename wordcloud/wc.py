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
			for (color, words) in color_to_words.items()]

		self.default_color_func = get_single_color_func(default_color)

	def get_color_func(self, word):
		"""Returns a single_color_func associated with the word"""
		try:
			color_func = next(
				color_func for (color_func, words) in self.color_func_to_words
				if word in words)
		except StopIteration:
			color_func = self.default_color_func

		return color_func

	def __call__(self, word, **kwargs):
		return self.get_color_func(word)(word, **kwargs)

class SimpleGroupedColorFunc(object):
	def __init__(self, color_to_words, default_color):
		self.word_to_color = {word: color
							  for (color, words) in color_to_words.items()
							  for word in words}

		self.default_color = default_color

	def __call__(self, word, **kwargs):
		return self.word_to_color.get(word, self.default_color)

alice_mask = np.array(Image.open("alice_mask.png"))

font_path = 'C:/Windows/fonts/h2gtre.ttf'

wordcloud = WordCloud(
	font_path = font_path,
	width = 800,
	height = 800,
	background_color="white",
	mask = alice_mask
)


grouped_color_func = GroupedColorFunc(color_to_words, default_color)
wordcloud.recolor(color_func=grouped_color_func)
plt.figure(figsize=(10, 10))
plt.imshow(wordcloud, interpolation="bilinear")
plt.axis("off")
plt.show()

imple_color_func = SimpleGroupedColorFunc(color_to_words, default_color)
wordcloud.recolor(color_func=simple_color_func)
plt.figure(figsize=(10, 10))
plt.imshow(wordcloud, interpolation="bilinear")
plt.axis("off")
plt.show()
