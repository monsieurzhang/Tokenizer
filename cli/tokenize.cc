#include <iostream>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include <onmt/Tokenizer.h>

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  po::options_description desc("Tokenization");
  desc.add_options()
    ("help,h", "display available options")
    ("mode,m", po::value<std::string>()->default_value("conservative"), "Define how aggressive should the tokenization be: 'aggressive' only keeps sequences of letters/numbers, 'conservative' allows mix of alphanumeric as in: '2,000', 'E65', 'soft-landing'")
    ("joiner_annotate,j", po::bool_switch()->default_value(false), "include joiner annotation using 'joiner' character")
    ("joiner", po::value<std::string>()->default_value(onmt::Tokenizer::joiner_marker), "character used to annotate joiners")
    ("joiner_new", po::bool_switch()->default_value(false), "in joiner_annotate mode, 'joiner' is an independent token")
    ("case_feature,c", po::bool_switch()->default_value(false), "lowercase corpus and generate case feature")
    ("segment_case", po::bool_switch()->default_value(false), "Segment case feature, splits AbC to Ab C to be able to restore case")
    ("segment_numbers", po::bool_switch()->default_value(false), "Segment numbers into single digits")
    ("segment_alphabet", po::value<std::string>()->default_value(""), "comma-separated list of alphabets on which to segment all letters.")
    ("segment_alphabet_change", po::bool_switch()->default_value(false), "Segment if the alphabet changes between 2 letters.")
    ("bpe_model,bpe", po::value<std::string>()->default_value(""), "path to the BPE model")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cerr << desc << std::endl;
    return 1;
  }

  int flags = 0;
  if (vm["case_feature"].as<bool>())
    flags |= onmt::Tokenizer::Flags::CaseFeature;
  if (vm["joiner_annotate"].as<bool>())
    flags |= onmt::Tokenizer::Flags::JoinerAnnotate;
  if (vm["joiner_new"].as<bool>())
    flags |= onmt::Tokenizer::Flags::JoinerNew;
  if (vm["segment_case"].as<bool>())
    flags |= onmt::Tokenizer::Flags::SegmentCase;
  if (vm["segment_numbers"].as<bool>())
    flags |= onmt::Tokenizer::Flags::SegmentNumbers;
  if (vm["segment_alphabet_change"].as<bool>())
    flags |= onmt::Tokenizer::Flags::SegmentAlphabetChange;

  std::vector<std::string> alphabets_to_segment;
  boost::split(alphabets_to_segment,
               vm["segment_alphabet"].as<std::string>(),
               boost::is_any_of(","));

  onmt::Tokenizer* tokenizer = new onmt::Tokenizer(
    onmt::Tokenizer::mapMode.at(vm["mode"].as<std::string>()),
    flags,
    vm["bpe_model"].as<std::string>(),
    vm["joiner"].as<std::string>());

  for (const auto& alphabet : alphabets_to_segment)
  {
    if (!alphabet.empty() && !tokenizer->add_alphabet_to_segment(alphabet))
      std::cerr << "WARNING: " << alphabet << " alphabet is not supported" << std::endl;
  }

  std::string line;

  while (std::getline(std::cin, line))
  {
    if (!line.empty())
      std::cout << reinterpret_cast<onmt::ITokenizer*>(tokenizer)->tokenize(line);

    std::cout << std::endl;
  }

  return 0;
}
