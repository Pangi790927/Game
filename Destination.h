#ifndef DESTINATION_H
#define DESTINATION_H

class Destination {
public:
	std::vector<Math::Point2i> path;
	Math::Point2i finish = Math::Point2i();
	int next = 0;

	void clearPath() {
		next = 0;
		path.clear();
	}

	void setFinish (Math::Point2i dest) {
		clearPath();
		finish = dest;
	}

	Math::Point2i getNext() {
		if (next < path.size()) {
			return path[next];
		}
		return Math::Point2i();
	}

	Math::Point2i advance() {
		if (next < path.size()) {
			return path[next++];
		}
		return Math::Point2i();
	}

	bool finishedPath() {
		return next >= path.size();
	}
};

#endif