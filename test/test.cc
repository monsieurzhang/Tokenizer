#include <memory>

#include <gtest/gtest.h>

#include <onmt/Tokenizer.h>
#include <onmt/Alphabet.h>

using namespace onmt;

static std::string data_dir;

static std::string get_data(const std::string& path) {
  return data_dir + "/" + path;
}

static void test_tok(std::unique_ptr<ITokenizer>& tokenizer,
                     const std::string& in,
                     const std::string& expected,
                     bool detokenize = false) {
  auto joined_tokens = tokenizer->tokenize(in);
  EXPECT_EQ(expected, joined_tokens);
  if (detokenize) {
    auto detok = tokenizer->detokenize(joined_tokens);
    EXPECT_EQ(in, detok);
  }
}

static void test_tok_and_detok(std::unique_ptr<ITokenizer>& tokenizer,
                               const std::string& in,
                               const std::string& expected) {
  return test_tok(tokenizer, in, expected, true);
}

TEST(TokenizerTest, BasicConservative) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Conservative));
  test_tok(tokenizer,
           "Your Hardware-Enablement Stack (HWE) is supported until April 2019.",
           "Your Hardware-Enablement Stack ( HWE ) is supported until April 2019 .");
}
TEST(TokenizerTest, ConservativeEmpty) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Conservative));
  test_tok(tokenizer, "", "");
}

TEST(TokenizerTest, BasicSpace) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Space));
  test_tok(tokenizer,
           "49th meeting Social and human rights questions: human rights [14 (g)]",
           "49th meeting Social and human rights questions: human rights [14 (g)]");
}
TEST(TokenizerTest, SpaceEmpty) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Space));
  test_tok(tokenizer, "", "");
}
TEST(TokenizerTest, SpaceSingle) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Space));
  test_tok(tokenizer, "Hello", "Hello");
}
TEST(TokenizerTest, SpaceLeading) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Space));
  test_tok(tokenizer, " Hello", "Hello");
}
TEST(TokenizerTest, SpaceTrailing) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Space));
  test_tok(tokenizer, "Hello ", "Hello");
}
TEST(TokenizerTest, SpaceDuplicated) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Space));
  test_tok(tokenizer, "  Hello   World ", "Hello World");
}

TEST(TokenizerTest, BasicJoiner) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Aggressive, Tokenizer::Flags::JoinerAnnotate));
  test_tok_and_detok(tokenizer,
                     "Isn't it so-greatly working?",
                     "Isn ￭'￭ t it so ￭-￭ greatly working ￭?");
  test_tok_and_detok(tokenizer, "MP3", "MP ￭3");
  test_tok_and_detok(tokenizer, "A380", "A ￭380");
  test_tok_and_detok(tokenizer, "$1", "$￭ 1");
}

TEST(TokenizerTest, BasicSpaceWithFeatures) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Space, Tokenizer::Flags::CaseFeature));
  test_tok(tokenizer,
           "49th meeting Social and human rights questions: human rights [14 (g)]",
           "49th￨L meeting￨L social￨C and￨L human￨L rights￨L questions:￨L human￨L rights￨L [14￨N (g)]￨L");
}

TEST(TokenizerTest, ProtectedSequence) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Conservative, Tokenizer::Flags::JoinerAnnotate));
  test_tok_and_detok(tokenizer, "｟1,023｠km", "｟1,023｠￭ km");
  test_tok_and_detok(tokenizer, "A｟380｠", "A ￭｟380｠");
  test_tok_and_detok(tokenizer, "｟1,023｠｟380｠", "｟1,023｠￭ ｟380｠");
  test_tok_and_detok(tokenizer, "｟1023｠.", "｟1023｠ ￭.");
  test_tok_and_detok(tokenizer, "$｟0.23｠", "$￭ ｟0.23｠");
  test_tok_and_detok(tokenizer, "｟0.23｠$", "｟0.23｠ ￭$");
  test_tok_and_detok(tokenizer, "｟US$｠23", "｟US$｠￭ 23");
  test_tok_and_detok(tokenizer, "1｟ABCD｠0", "1 ￭｟ABCD｠￭ 0");
}

TEST(TokenizerTest, ProtectedSequenceAggressive) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Aggressive, Tokenizer::Flags::JoinerAnnotate));
  test_tok_and_detok(tokenizer, "｟1,023｠km", "｟1,023｠￭ km");
  test_tok_and_detok(tokenizer, "A｟380｠", "A ￭｟380｠");
  test_tok_and_detok(tokenizer, "｟1,023｠｟380｠", "｟1,023｠￭ ｟380｠");
  test_tok_and_detok(tokenizer, "｟1023｠.", "｟1023｠ ￭.");
  test_tok_and_detok(tokenizer, "$｟0.23｠", "$￭ ｟0.23｠");
  test_tok_and_detok(tokenizer, "｟0.23｠$", "｟0.23｠ ￭$");
  test_tok_and_detok(tokenizer, "｟US$｠23", "｟US$｠￭ 23");
  test_tok_and_detok(tokenizer, "1｟ABCD｠0", "1 ￭｟ABCD｠￭ 0");
}

TEST(TokenizerTest, ProtectedSequenceJoinerNew) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Conservative,
                  Tokenizer::Flags::JoinerAnnotate | Tokenizer::Flags::JoinerNew));
  test_tok_and_detok(tokenizer, "｟1,023｠km", "｟1,023｠ ￭ km");
  test_tok_and_detok(tokenizer, "A｟380｠", "A ￭ ｟380｠");
  test_tok_and_detok(tokenizer, "｟1,023｠｟380｠", "｟1,023｠ ￭ ｟380｠");
  test_tok_and_detok(tokenizer, "｟1023｠.", "｟1023｠ ￭ .");
}

TEST(TokenizerTest, Substitutes) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Conservative));
  test_tok(tokenizer,
           "test￭ protect￨, ：, and ％ or ＃...",
           "test ■ protect │ , : , and % or # . . .");
}

TEST(TokenizerTest, NoSubstitution) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Conservative, Tokenizer::Flags::NoSubstitution));
  test_tok(tokenizer,
           "test￭ protect￨, ：, and ％ or ＃...",
           "test ￭ protect ￨ , ： , and ％ or ＃ . . .");
}

TEST(TokenizerTest, CombiningMark) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Conservative, Tokenizer::Flags::JoinerAnnotate));
  test_tok_and_detok(tokenizer,
                     "वर्तमान लिपि (स्क्रिप्ट) खो जाएगी।",
                     "वर्तमान लिपि (￭ स्क्रिप्ट ￭) खो जाएगी ￭।");
}

TEST(TokenizerTest, CaseFeature) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Conservative,
                  Tokenizer::Flags::CaseFeature | Tokenizer::Flags::JoinerAnnotate));
  // Note: in C literal strings, \ is escaped by another \.
  test_tok(tokenizer,
           "test \\\\\\\\a Capitalized lowercased UPPERCASÉ miXêd - cyrillic-Б",
           "test￨L \\￨N ￭\\￨N ￭\\￨N ￭\\￭￨N a￨L capitalized￨C lowercased￨L uppercasé￨U mixêd￨M -￨N cyrillic-б￨M");
}

TEST(TokenizerTest, SegmentCase) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Conservative,
                  Tokenizer::Flags::CaseFeature | Tokenizer::Flags::JoinerAnnotate | Tokenizer::Flags::SegmentCase));
  test_tok_and_detok(tokenizer, "WiFi", "wi￭￨C fi￨C");
}

TEST(TokenizerTest, SegmentNumbers) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Aggressive,
                  Tokenizer::Flags::JoinerAnnotate | Tokenizer::Flags::SegmentNumbers));
  test_tok_and_detok(tokenizer,
                     "1984 mille neuf cent quatrevingt-quatre",
                     "1￭ 9￭ 8￭ 4 mille neuf cent quatrevingt ￭-￭ quatre");
}

TEST(TokenizerTest, GetAlphabet) {
  for (const auto& a : onmt::alphabet_ranges) {
    const auto& range = a.first;
    const auto& name = a.second;

    EXPECT_EQ(name, onmt::get_alphabet(range.first));
    EXPECT_EQ(name, onmt::get_alphabet(range.second));
  }
}

TEST(TokenizerTest, SegmentAlphabet) {
  auto tokenizer_raw = new Tokenizer(Tokenizer::Mode::Conservative, Tokenizer::Flags::JoinerAnnotate);
  tokenizer_raw->add_alphabet_to_segment("Han");
  auto tokenizer = std::unique_ptr<ITokenizer>(tokenizer_raw);
  test_tok_and_detok(tokenizer, "rawБ", "rawБ");
  test_tok(tokenizer,
           "有入聲嘅唐話往往有陽入對轉，即係入聲韻尾同鼻音韻尾可以轉化。比如粵語嘅「抌」（dam）「揼」（dap），意思接近，意味微妙，區別在於-m同-p嘅轉換。",
           "有￭ 入￭ 聲￭ 嘅￭ 唐￭ 話￭ 往￭ 往￭ 有￭ 陽￭ 入￭ 對￭ 轉 ￭，￭ 即￭ 係￭ 入￭ 聲￭ 韻￭ 尾￭ 同￭ 鼻￭ 音￭ 韻￭ 尾￭ 可￭ 以￭ 轉￭ 化 ￭。￭ 比￭ 如￭ 粵￭ 語￭ 嘅 ￭「￭ 抌 ￭」 ￭（￭ dam ￭） ￭「￭ 揼 ￭」 ￭（￭ dap ￭） ￭，￭ 意￭ 思￭ 接￭ 近 ￭，￭ 意￭ 味￭ 微￭ 妙 ￭，￭ 區￭ 別￭ 在￭ 於-m同-p嘅￭ 轉￭ 換 ￭。");
}

TEST(TokenizerTest, SegmentAlphabetChange) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Conservative, Tokenizer::Flags::SegmentAlphabetChange));
  test_tok(tokenizer, "rawБ", "raw Б");
}

TEST(TokenizerTest, BPEBasic) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Conservative, Tokenizer::Flags::JoinerAnnotate,
                  get_data("bpe-models/testcode")));
  test_tok_and_detok(tokenizer,
                     "abcdimprovement联合国",
                     "a￭ b￭ c￭ d￭ impr￭ ovemen￭ t￭ 联合￭ 国");
}

TEST(TokenizerTest, BPEModePrefix) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Aggressive, Tokenizer::Flags::None,
                  get_data("bpe-models/codes_prefix.fr")));
  test_tok(tokenizer,
           "Seulement seulement il vais nonseulement seulementnon à Verdun",
           "S e u lement seulement il v ais n on se u lement seulement n on à V er d un");
}

TEST(TokenizerTest, BPEModeNofix) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Aggressive, Tokenizer::Flags::None,
                  get_data("bpe-models/codes_nofix.fr")));
  test_tok(tokenizer,
           "Seulement seulement il vais nonseulement seulementnon à Verdun",
           "S e u lement seulement il v ais n on seulement seulement n on à V er d un");
}

TEST(TokenizerTest, BPEModeBoth) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Aggressive, Tokenizer::Flags::None,
                  get_data("bpe-models/codes_bothfix.fr")));
  test_tok(tokenizer,
           "Seulement seulement il vais nonseulement seulementnon à Verdun",
           "S eu lement seulement il va is n on s eu lement seu l emen t n on à V er du n");
}

TEST(TokenizerTest, BPECaseInsensitive) {
  auto tokenizer = std::unique_ptr<ITokenizer>(
    new Tokenizer(Tokenizer::Mode::Aggressive, Tokenizer::Flags::None,
                  get_data("bpe-models/codes_suffix_case_insensitive.fr")));
  test_tok(tokenizer,
           "Seulement seulement il vais nonseulement seulementnon à Verdun",
           "Seulement seulement il va is n on seulement seu l em ent n on à Ver d un");
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  assert(argc == 2);
  data_dir = argv[1];
  return RUN_ALL_TESTS();
}
