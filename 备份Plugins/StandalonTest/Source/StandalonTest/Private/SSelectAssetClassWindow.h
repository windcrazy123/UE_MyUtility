#pragma once
#include "IAssetTools.h"

class FClassFilterTreeNode : public TSharedFromThis<FClassFilterTreeNode>
{
public:
	FString CategoryName;
	FString DisplayName;          // 显示名称（如 "Materials"）
	FString ClassName;            // 类名（如 "Material"）
	TArray<TSharedPtr<FClassFilterTreeNode>> Children; // 子节点
	bool bIsCategory = false;     // 是否为分类节点（不可勾选）
	bool bIsChecked = false;      // 是否选中

	FClassFilterTreeNode(const FString& InDisplayName, const FString& InClassName, bool bInIsCategory = false)
		: DisplayName(InDisplayName), ClassName(InClassName), bIsCategory(bInIsCategory)
	{
		CategoryName = "NotDefine";
	}
	FClassFilterTreeNode(const FString& InDisplayName, const FString& InClassName, const FString& InCategory, bool bInIsCategory)
		: CategoryName(InCategory), DisplayName(InDisplayName), ClassName(InClassName), bIsCategory(bInIsCategory)
	{
	}
};

class SSelectAssetClassWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSelectAssetClassWindow)
		:  _ParentWindow(){}
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	const TArray<FString>& GetSelectedClasses() const { return SelectedClasses; }


	// 获取用户选择的类名
	TArray<FString> GetSelectedClassNames()// const
	{
		TArray<FString> Result;
		ForEachNode(RootNode, [&](TSharedPtr<FClassFilterTreeNode> Node) {
			if (!Node->bIsCategory && Node->bIsChecked)
			{
				Result.Add(Node->ClassName);
			}
		});
		return Result;
	}
private:
	FReply OnConfirmClicked()
	{
		CloseWindow(true);
		return FReply::Handled();
	}
	FReply OnCancelClicked()
	{
		CloseWindow(false);
		return FReply::Handled();
	}
	void CloseWindow(bool bConfirmed)
	{
		if(!bConfirmed)
		{
			SelectedClasses.Empty();
		}
		if(ParentWindow.IsValid())
		{
			ParentWindow.Pin()->RequestDestroyWindow();
		}
	}
	
	// 生成列表行
	TSharedRef<ITableRow> GenerateRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable);

	// 初始化分类结构（示例，按引擎常用分类）
	TSharedPtr<FClassFilterTreeNode> BuildClassFilterTree();
	// 搜索过滤
	void OnSearchTextChanged(const FText& InText);
	// 生成树节点行
	TSharedRef<ITableRow> GenerateTreeRow(TSharedPtr<FClassFilterTreeNode> Node, const TSharedRef<STableViewBase>& OwnerTable);
	void OnTreeRowReleased(const TSharedRef<ITableRow>& TableRow);
	// 递归获取子节点
	void OnGetChildren(TSharedPtr<FClassFilterTreeNode> Parent, TArray<TSharedPtr<FClassFilterTreeNode>>& OutChildren);
	// 递归检查子节点是否匹配搜索
	bool AnyChildMatches(TSharedPtr<FClassFilterTreeNode> Node, const FString& SearchText);
	// 重置过滤器
	FReply OnResetFilters();
	// 递归遍历节点
	template<typename Func>
	void ForEachNode(TSharedPtr<FClassFilterTreeNode> Node, Func&& Predicate)
	{
		Predicate(Node);
		for (const auto& Child : Node->Children)
		{
			ForEachNode(Child, Predicate);
		}
	}

private:
	TSharedPtr<FClassFilterTreeNode> RootNode;
	TArray<TSharedPtr<FClassFilterTreeNode>> SourceNodes;
	TSharedPtr<SSearchBox> SearchBox;
	TMap<TSharedRef<ITableRow>, TSharedPtr<SCheckBox>> CheckedBoxes;
	TSharedPtr<STreeView<TSharedPtr<FClassFilterTreeNode>>> TreeView;
	
	TArray<FString> SelectedClasses;
	TArray<TSharedPtr<FString>> FilteredClasses;
	TWeakPtr<SWindow> ParentWindow;
	EAssetClassAction AssetClassAction = EAssetClassAction::ViewAsset;
	TMap<FName, TSharedPtr<FClassFilterTreeNode>> AssetFilterCategories;
};
