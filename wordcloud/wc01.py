
from wordcloud import WordCloud
#from wordcloud import STOPWORDS

texts = ['이것 은 예문 입니다', '여러분 의 문장을 넣 으세요']
keywords = {'이것':5, '예문':3, '단어':5, '빈도수':3}

wordcloud = WordCloud(
    font_path = font_path,
    width = 800,
    height = 800
)
wordcloud = wordcloud.generate_from_text(texts)
wordcloud = wordcloud.generate_from_frequencies(keywords)

stopwords = {'은', '입니다'}

wordcloud = WordCloud(stopwords=stopwords)
