#pragma once

/// <summary>
/// 字典树
/// </summary>
class Trie
{
	struct TrieNode
	{
		TrieNode*	m_branch[26];//26个分支
		CHAR*		m_pzText;//分支到根节点的单词
	};
public:
	Trie();
	~Trie();
	BOOL Insert(LPCSTR pzValue, LPCSTR pzKey);
	BOOL Remove(LPCSTR pzValue);
private:
	TrieNode* m_pRoot;
};