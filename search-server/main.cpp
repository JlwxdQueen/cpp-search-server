#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

vector<string> SplitIntoWords(const string &text);

struct Document
{
    int id;
    double relevance;
};

struct Query
{
    set<string> positive_words;
    set<string> negative_words;
};

class SearchServer
{
public:
    void SetStopWords(const string &text)
    {
        for (const string &word : SplitIntoWords(text))
        {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string &document)
    {
        const vector<string> words = SplitIntoWordsNoStop(document);

        for (const string &word : words)
        {
            word_to_document_freqs_[word][document_id] += 1.0 / words.size();
        }

        document_count_++;
    }

    vector<Document> FindTopDocuments(const string &raw_query) const
    {
        const Query query = ParseQuery(raw_query);

        if (query.positive_words.empty() && query.negative_words.empty())
        {
            return {};
        }

        auto matched_documents = FindAllDocuments(query);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document &lhs, const Document &rhs)
             {
                 return lhs.relevance > rhs.relevance;
             });

        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT)
        {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return matched_documents;
    }

private:
    unordered_map<string, unordered_map<int, double>> word_to_document_freqs_;
    unordered_map<int, int> document_ids_count_;
    int document_count_ = 0;
    set<string> stop_words_;

    bool IsStopWord(const string &word) const
    {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string &text) const
    {
        vector<string> words;
        for (const string &word : SplitIntoWords(text))
        {
            if (!IsStopWord(word))
            {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string &text) const
    {
        Query query;
        for (const string &word : SplitIntoWordsNoStop(text))
        {
            if (word[0] == '-')
            {
                query.negative_words.insert(word.substr(1));
            }
            else
            {
                query.positive_words.insert(word);
            }
        }
        return query;
    }

    double CalculateIdf(const string &word) const
    {
        return log(static_cast<double>(document_count_) / word_to_document_freqs_.at(word).size());
    }

    vector<Document> FindAllDocuments(const Query &query) const
    {
        vector<Document> matched_documents;
        unordered_map<int, double> document_to_relevance;

        for (const string &word : query.positive_words)
        {
            double idf = CalculateIdf(word);

            for (const auto &[document_id, term_freq] : word_to_document_freqs_.at(word))
            {
                if (query.negative_words.count(word) == 0)
                {
                    double tfidf = term_freq * idf;
                    document_to_relevance[document_id] += tfidf;
                }
            }
        }

        for (const auto &[document_id, relevance] : document_to_relevance)
        {
            matched_documents.push_back({document_id, relevance});
        }

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document &lhs, const Document &rhs)
             {
                 return lhs.relevance > rhs.relevance;
             });

        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT)
        {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return matched_documents;
    }
};

vector<string> SplitIntoWords(const string &text)
{
    vector<string> words;
    string word;
    for (const char c : text)
    {
        if (c == ' ')
        {
            if (!word.empty())
            {
                words.push_back(word);
                word.clear();
            }
        }
        else
        {
            word += c;
        }
    }
    if (!word.empty())
    {
        words.push_back(word);
    }

    return words;
}

string ReadLine()
{
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber()
{
    string s = ReadLine();
    return stoi(s);
}

SearchServer CreateSearchServer()
{
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id)
    {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main()
{
    SearchServer search_server = CreateSearchServer();

    string query = ReadLine();
    vector<Document> top_documents = search_server.FindTopDocuments(query);

    for (const auto &document : top_documents)
    {
        cout << "{ document_id = " << document.id << ", relevance = " << document.relevance << " }" << endl;
    }

    return 0;
}
