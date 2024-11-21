#pragma once
#include <vector>


namespace md::utils {
    template <typename T>
    class FixedVector {
    public:
        // void emplace_back(T value) {container.emplace_back(value);}
        size_t size() const { return container.size(); }
        void lock();

    private:
        std::vector<T> container{};
        bool locked = false;
    };


    template <typename Container>
    class PairIteratorBase {
    public:
        virtual ~PairIteratorBase() = default;
        using ContainerIt = typename Container::iterator;
        using value_type = std::pair<typename Container::value_type, typename Container::value_type>;
        using reference = std::pair<const typename Container::value_type&, const typename Container::value_type&>;
        using iterator_category = std::forward_iterator_tag;

        PairIteratorBase(Container& inner_container, Container& outer_container)
            : inner_container(inner_container), outer_container(outer_container),
              inner(inner_container.begin()), outer(outer_container.begin()) {}

        PairIteratorBase& operator++() {
            ++inner;
            if (inner == inner_container.end()) {
                ++outer;
                if (outer != outer_container.end()) {
                    reset_inner();
                }
            }
            return *this;
        }

        PairIteratorBase operator++(int) {
            PairIteratorBase temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const PairIteratorBase& other) const {
            return inner == other.inner && outer == other.outer;
        }

        bool operator!=(const PairIteratorBase& other) const {
            return !(*this == other);
        }

        reference operator*() const {
            return {*inner, *outer};
        }

    protected:
        virtual void reset_inner() = 0;
        Container& inner_container;
        Container& outer_container;
        ContainerIt inner;
        ContainerIt outer;
    };


    template <typename Container>
    class CrossPairIterator {
    public:
        class Iterator final : public PairIteratorBase<Container> {
        public:
            using PairIteratorBase<Container>::PairIteratorBase;

        protected:
            void reset_inner() override {
                this->inner = this->inner_container.begin();
            }
        };

        CrossPairIterator(Container& inner_container, Container& outer_container)
            : inner_container(inner_container), outer_container(outer_container) {}

        Iterator begin() {
            return Iterator(inner_container, outer_container);
        }

        Iterator end() {
            Iterator it(inner_container, outer_container);
            it.outer = outer_container.end();
            it.inner = inner_container.end();
            return it;
        }

    private:
        Container& inner_container;
        Container& outer_container;
    };


    template <typename Container>
    class UniquePairIterator {
    public:
        class Iterator final : public PairIteratorBase<Container> {
        public:
            using PairIteratorBase<Container>::PairIteratorBase;

        protected:
            void reset_inner() override {
                this->inner = std::next(this->outer);
            }
        };

        explicit UniquePairIterator(Container& container)
            : container(container) {}

        Iterator begin() {
            Iterator it(container, container);
            if (it.outer != container.end()) {
                it.inner = std::next(it.outer); // make sure first begin is not a pair with two identical elements
                if (it.inner == container.end() && ++it.outer != container.end()) {
                    it.inner = std::next(it.outer);
                }
            }
            return it;
        }

        Iterator end() {
            Iterator it(container, container);
            it.outer = container.end();
            it.inner = container.end();
            return it;
        }

    private:
        Container& container;
    };
}
