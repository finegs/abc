import logging

from lenlp import sparse

# 로거 세팅
logger = logging.getLogger("__root__")
# TODO : 
logger.setLevel(logging.DEBUG)

# 일반 핸들러, 포매터 세팅
formatter = logging.Formatter("%(asctime)s %(levelname)s:%(message)s")
handler = logging.StreamHandler()
handler.setFormatter(formatter)

# 크리티컬 이벤트에 대한 핸들러, 포매터 세팅
formatter_critical = logging.Formatter("!!!!!%(asctime)s %(levelname)s:%(message)s")
handler_critical = logging.FileHandler("log_event.log")
handler_critical.setLevel(logging.CRITICAL)
handler_critical.setFormatter(formatter_critical)

# 각 핸들러를 로거에 추가
logger.addHandler(handler)
logger.addHandler(handler_critical)

vectorizer = sparse.CountVectorizer(
	    ngram_range=(3, 5), # range of n-grams
	    analyzer="char_wb", # word, char, char_wb
	    normalize=True, # lowercase and strip accents
	    stop_words=["based"], # list of stop words
	
)

X = [
	"Hello World",
	"Rust based vectorizer"
]

matrix =  vectorizer.fit_transform(X)

