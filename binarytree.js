function Node(value, leftNode, rightNode, parentNode, depth){
	this.value = value;
	this.leftNode = leftNode;
	this.rightNode = rightNode;
	this.parentNode = parentNode;
	this.depth = depth;
}

var mSize;
var mDepth;
var mRootNode;

function BinaryTree(){
	mSize = 0;
	mDepth = -1;
}

function BinaryTree(rootValue){
	mSize = 1;
	mDepth = 0;
	mRootNode = new Node(rootValue, null, null, null, mDepth);
}

BinaryTree.prototype.getRootNode = function(){
	return mRootNode;
};

BinaryTree.prototype.insertNode = function(value){
	if(mRootNode.value != undefined){
		var newNode = new Node(value, null, null, null, null);
		insertNodeCore(mRootNode, newNode, 0);
	}else{
		mRootNode = new Node(value, null, null, null, 0);
		mDepth++;
		mSize++;
	}
};

function insertNodeCore(node, newNode, nodeDepth){
	nodeDepth++;
	if(node.value > newNode.value){
		if(node.leftNode == null){
			node.leftNode = new Node(newNode.value, null, null, node, nodeDepth);
			mSize++;
			if(nodeDepth > mDepth){
				mDepth = nodeDepth;
			}
			return;
		}
		insertNodeCore(node.leftNode, newNode, nodeDepth);
	}else if(node.value < newNode.value){
		if(node.rightNode == null){
			node.rightNode = new Node(newNode.value, null, null, node, nodeDepth);
			mSize++;
			if(nodeDepth > mDepth){
				mDepth = nodeDepth;
			}
			return;
		}
		insertNodeCore(node.rightNode, newNode, nodeDepth);
	}else{
		return;
	}
}

BinaryTree.prototype.search = function(value){
	var currentNode = mRootNode;
	while(currentNode != null){
		if(value == currentNode.value){
			return currentNode;
		}else if(value < currentNode.value){
			currentNode = currentNode.leftNode;
		}else{
			currentNode = currentNode.rightNode;
		}
	}
	return null;
};

BinaryTree.prototype.getPreorderArray = function(){
	var nodeArray = [];
	preorderCore(nodeArray, mRootNode);
	return nodeArray;
};

function preorderCore(nodeArray, node){
	nodeArray.push(node);
	if(node.leftNode != null){
		preorderCore(nodeArray, node.leftNode);
	}

	if(node.rightNode != null){
		preorderCore(nodeArray, node.rightNode);
	}
}

BinaryTree.prototype.getInorderArray = function(){
	var nodeArray = [];
	inorderCore(nodeArray, mRootNode);
	return nodeArray;
}

function inorderCore(nodeArray, node){
	if(node.leftNode != null){
		inorderCore(nodeArray, node.leftNode);
	}

	nodeArray.push(node);

	if(node.rightNode != null){
		inorderCore(nodeArray, node.rightNode);
	}
}

BinaryTree.prototype.getPostorderArray = function(){
	var nodeArray = [];
	postorderCore(nodeArray, mRootNode);
	return nodeArray;
}

function postorderCore(nodeArray, node){
	if(node.leftNode != null){
		postorderCore(nodeArray, node.leftNode);
	}

	if(node.rightNode != null){
		postorderCore(nodeArray, node.rightNode);
	}
	nodeArray.push(node);
}





