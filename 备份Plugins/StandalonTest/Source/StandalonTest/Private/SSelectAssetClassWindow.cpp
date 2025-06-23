#include "SSelectAssetClassWindow.h"

#include "AssetDefinitionRegistry.h"
#include "AssetToolsModule.h"
#include "Filters/CustomClassFilterData.h"
#include "Widgets/Input/SSearchBox.h"

//可参考D:\UE_5.4\Engine\Plugins\Editor\GameplayTagsEditor\Source\GameplayTagsEditor\Private\SGameplayTagWidget.cpp

void SSelectAssetClassWindow::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;

	//方法一，不知道如何将可作为uasset文件的类过滤出来
	/*// 获取所有可过滤的资产类
	TArray<UClass*> AllAssetClasses;
	GetDerivedClasses(UObject::StaticClass(), AllAssetClasses, true);

	// 过滤掉不可作为资产的类（如抽象类、蓝图类等）
	AllAssetClasses.RemoveAll([](UClass* Class) {
		return !Class->IsAsset() || Class->HasAnyClassFlags(CLASS_Abstract);
	});

	// 构建列表项
	for (UClass* Class : AllAssetClasses)
	{
		UE_LOG(LogTemp, Log, TEXT("Class: %s"), *Class->GetName())
		FilteredClasses.Add(MakeShared<FString>(Class->GetName()));
	}*/

	RootNode = BuildClassFilterTree();
	SourceNodes = RootNode->Children;

	SearchBox  = SNew(SSearchBox)
		.HintText(FText::FromString("Search..."))
		.OnTextChanged(this, &SSelectAssetClassWindow::OnSearchTextChanged);
	
	ChildSlot
	[
		SNew(SBox)
		.MaxDesiredHeight(500)
		[
			SNew(SVerticalBox)
			+  SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString("Reset Filters"))
					.OnClicked(this, &SSelectAssetClassWindow::OnResetFilters)
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SearchBox.ToSharedRef()
				]
			]
			/*+ SVerticalBox::Slot()
			.FillHeight(0.9f)
			[
				SNew(SListView<TSharedPtr<FString>>)
				.ListItemsSource(&FilteredClasses)
				.OnGenerateRow(this, &SSelectAssetClassWindow::GenerateRow)
				.SelectionMode(ESelectionMode::None)
			]*/
			+ SVerticalBox::Slot()
			.FillHeight(0.9f)
			[
				/*SNew(SBox)
				.Padding(5.f)
				.MaxDesiredHeight(500)
				[*/
					SAssignNew(TreeView, STreeView<TSharedPtr<FClassFilterTreeNode>>)
					.TreeItemsSource(&SourceNodes)
					.OnGenerateRow(this, &SSelectAssetClassWindow::GenerateTreeRow)
					.OnRowReleased(this, &SSelectAssetClassWindow::OnTreeRowReleased)
					.OnGetChildren(this, &SSelectAssetClassWindow::OnGetChildren)
					.SelectionMode(ESelectionMode::None)
				//]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+  SHorizontalBox::Slot()
				.Padding(5)
				[
					SNew(SButton)
					.Text(FText::FromString("Confirm"))
					.OnClicked(this, &SSelectAssetClassWindow::OnConfirmClicked)
				]
				+ SHorizontalBox::Slot()
				.Padding(5)
				[
					SNew(SButton)
					.Text(FText::FromString("Cancel"))
					.OnClicked(this, &SSelectAssetClassWindow::OnCancelClicked)
				]
			]
		]
	];
}

TSharedRef<ITableRow> SSelectAssetClassWindow::GenerateRow(TSharedPtr<FString> Item,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
	[
		SNew(SCheckBox)
		.IsChecked_Lambda([this, Item]()
		{
			return SelectedClasses.Contains(*Item) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
		})
		.OnCheckStateChanged_Lambda([this, Item](ECheckBoxState NewState)
		{
			if (NewState == ECheckBoxState::Checked)
			{
				SelectedClasses.Add(*Item);
			}
			else
			{
				SelectedClasses.Remove(*Item);
			}
		})
		.Content()
		[
			SNew(STextBlock)
			.Text(FText::FromString(*Item))
		]
	];
}

TSharedPtr<FClassFilterTreeNode> SSelectAssetClassWindow::BuildClassFilterTree()
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	TArray<TObjectPtr<UAssetDefinition>> AssetDefinitions = UAssetDefinitionRegistry::Get()->GetAllAssetDefinitions();
	const TSharedRef<FPathPermissionList>& AssetClassPermissionList = AssetToolsModule.Get().GetAssetClassPathPermissionList(AssetClassAction);
	
	
	auto Root = MakeShared<FClassFilterTreeNode>("Root", "", true);

	// Basic
	//auto BasicCategory = MakeShared<FClassFilterTreeNode>("Basic", "", true);
	for (auto& AssetDefinition : AssetDefinitions)
	{
		TSoftClassPtr<UObject> AssetClass = AssetDefinition->GetAssetClass();
		if(AssetClass.IsValid() && AssetClassPermissionList->PassesFilter(AssetClass.ToString())/* && AssetClass->GetClassPathName().ToString().Contains("Texture")*/)
		{
			const TSharedRef<FAssetFilterDataCache> FilterCache = AssetDefinition->GetFilters();
				
			for (const FAssetFilterData& FilterData : FilterCache->Filters)
			{
				ensureMsgf(FilterData.FilterCategories.Num() > 0, TEXT("%s is missing Filter Categories, without any filter categories we can't display this filter."), *FilterData.Name);
					
				// Convert the AssetTypeAction to an FCustomClassFilterData and add it to our list
				TSharedRef<FCustomClassFilterData> CustomClassFilterData = MakeShared<FCustomClassFilterData>(AssetDefinition, FilterData);
				FString ClassName = AssetClass.ToString();
				
				// Add the Basic to our list
				for (const FAssetCategoryPath& CategoryPath : FilterData.FilterCategories)
				{
					TSharedPtr<FClassFilterTreeNode> FilterCategory = AssetFilterCategories.FindRef(CategoryPath.GetCategory());
					FString CategoryName = CategoryPath.GetCategoryText().ToString();

					if(CategoryName != "Basic") continue;
					if (!FilterCategory.IsValid())
					{
						FilterCategory = MakeShared<FClassFilterTreeNode>(FPaths::GetExtension(ClassName), ClassName, "Basic", false);
						
						Root->Children.Add(FilterCategory);
					}
					
					//CustomClassFilterData->AddCategory(FilterCategory);
				}
				//CustomClassFilters.Add(CustomClassFilterData);

				// Add the category to our list
				for (const FAssetCategoryPath& CategoryPath : FilterData.FilterCategories)
				{
					TSharedPtr<FClassFilterTreeNode> FilterCategory = AssetFilterCategories.FindRef(CategoryPath.GetCategory());
					FString CategoryName = CategoryPath.GetCategoryText().ToString();
					
					if(CategoryName == "Basic") continue;
					if (!FilterCategory.IsValid())
					{
						FilterCategory = MakeShared<FClassFilterTreeNode>(CategoryPath.GetCategoryText().ToString(), "", true);
						AssetFilterCategories.Add(CategoryPath.GetCategory(), FilterCategory);
						
						Root->Children.Add(FilterCategory);
					}
					FilterCategory->Children.Add(MakeShared<FClassFilterTreeNode>(FPaths::GetExtension(ClassName), ClassName));
				}
			}
			
			//FString ClassName = AssetClass.ToString();
			//BasicCategory->Children.Add(MakeShared<FClassFilterTreeNode>(FPaths::GetExtension(ClassName), ClassName));
			//UE_LOG(LogTemp, Log, TEXT("Texture: %s"), *ClassName)
		}
	}
	//Root->Children.Add(BasicCategory);

	// Animation
	/*auto AnimCategory = MakeShared<FClassFilterTreeNode>("Animation", "", true);
	AnimCategory->Children.Add(MakeShared<FClassFilterTreeNode>("Skeletal Mesh", "SkeletalMesh"));
	AnimCategory->Children.Add(MakeShared<FClassFilterTreeNode>("Animation Blueprint", "AnimBlueprint"));
	Root->Children.Add(AnimCategory);*/

	// 添加更多分类...
	return Root;
}

// @TODO: 构建并排序
TSharedRef<ITableRow> SSelectAssetClassWindow::GenerateTreeRow(TSharedPtr<FClassFilterTreeNode> Node, const TSharedRef<STableViewBase>& OwnerTable)
{
	auto TableRow = SNew(STableRow<TSharedPtr<FClassFilterTreeNode>>, OwnerTable);
	TSharedPtr<SCheckBox> CheckBox;
	TableRow->SetContent(
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(CheckBox, SCheckBox)
			.IsChecked(Node->bIsCategory ? ECheckBoxState::Unchecked : (Node->bIsChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked))
			.OnCheckStateChanged_Lambda([this, Node, CheckBox](ECheckBoxState NewState)
			{
				if (Node->bIsCategory) return;
				
				if(NewState == ECheckBoxState::Checked)
				{
					SelectedClasses.Add(Node->ClassName);
					//CheckedBoxes.Add(CheckBox);
					Node->bIsChecked = true;
				}else
				{
					SelectedClasses.Remove(Node->ClassName);
					//CheckedBoxes.Remove(CheckBox);
					Node->bIsChecked = false;
					UE_LOG(LogTemp, Log, TEXT("Uncheck: %s"), *Node->ClassName)
				}
			})
			.IsEnabled(!Node->bIsCategory)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.9f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(Node->DisplayName))
		]
		/*+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SExpanderArrow, TableRow)
			.IndentAmount(12)
		]*/
	);

	CheckedBoxes.Emplace(TableRow, CheckBox);

	return TableRow;

	/*return SNew(STableRow<TSharedPtr<FClassFilterTreeNode>>, OwnerTable)
		[
			SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SCheckBox)
			.IsChecked(Node->bIsCategory ? ECheckBoxState::Unchecked : (Node->bIsChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked))
			.OnCheckStateChanged_Lambda([this, Node](ECheckBoxState NewState)
			{
				if (Node->bIsCategory) return;
				
				if(NewState == ECheckBoxState::Checked)
				{
					SelectedClasses.Add(Node->ClassName);
					//CheckedBoxes.Add(CheckBox);
					Node->bIsChecked = true;
				}else
				{
					SelectedClasses.Remove(Node->ClassName);
					//CheckedBoxes.Remove(CheckBox);
					Node->bIsChecked = false;
				}
			})
			.IsEnabled(!Node->bIsCategory)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.9f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(Node->DisplayName))
		]
		];*/
}

void SSelectAssetClassWindow::OnTreeRowReleased(const TSharedRef<ITableRow>& TableRow)
{
	CheckedBoxes.FindAndRemoveChecked(TableRow);
}


// @TODO: 现在是预构建树，在搜索时没办法动态构建，需要动态构建每个节点
void SSelectAssetClassWindow::OnSearchTextChanged(const FText& InText)
{
	OnResetFilters();
	
	if(InText.IsEmpty())
	{
		SourceNodes.Empty();
		SourceNodes = RootNode->Children;
		this->TreeView->RequestTreeRefresh();
	}else
	{
		const FString SearchText = InText.ToString().ToLower();
		SourceNodes.Empty();

		ForEachNode(RootNode, [&](TSharedPtr<FClassFilterTreeNode> Node)
		{
			if(Node->DisplayName == "Root") return;
		
			//const bool bMatchesSearch = Node->DisplayName.ToLower().Contains(SearchText);
			if (/*bMatchesSearch || */AnyChildMatches(Node, SearchText))//一定不要把构建好的父节点和子节点一起加入过滤结果
			{
				SourceNodes.Add(Node);
			}
		});

		for(auto filter : SourceNodes)
		{
			UE_LOG(LogTemp, Log, TEXT("SearchText: %s"), *filter->DisplayName)
		}

		//TreeView->SetTreeItemsSource(&FilteredNodes);

		this->TreeView->RequestTreeRefresh();
	}

	
	UE_LOG(LogTemp, Log, TEXT("SearchText: %d"), CheckedBoxes.Num())
}

void SSelectAssetClassWindow::OnGetChildren(TSharedPtr<FClassFilterTreeNode> Parent,
	TArray<TSharedPtr<FClassFilterTreeNode>>& OutChildren)
{
	OutChildren = Parent->Children;
}

bool SSelectAssetClassWindow::AnyChildMatches(TSharedPtr<FClassFilterTreeNode> Node, const FString& SearchText)
{
	if(Node->CategoryName == "Basic" && Node->DisplayName.ToLower().Contains(SearchText)) return true;
	
	for (const auto& Child : Node->Children)
	{
		if (Child->DisplayName.ToLower().Contains(SearchText) || AnyChildMatches(Child, SearchText)){
			return true;
		}
	}
	return false;
}

FReply SSelectAssetClassWindow::OnResetFilters()
{
	UE_LOG(LogTemp, Log, TEXT("ResetFilters:%d"),CheckedBoxes.Num())
	for(auto CheckBox : CheckedBoxes)
	{
		if(CheckBox.Value->GetCheckedState() == ECheckBoxState::Checked)
			CheckBox.Value->ToggleCheckedState();
	}
	//CheckedBoxes.Empty();
	SelectedClasses.Empty();
	return FReply::Handled();
}