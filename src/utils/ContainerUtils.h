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
    class PairIterator {
        using ContainerIt = typename Container::iterator;

    public:
        class Iterator {
        public:
            using value_type = std::pair<typename Container::value_type, typename Container::value_type>;
            using reference = std::pair<const typename Container::value_type&, const typename Container::value_type&>;
            using iterator_category = std::forward_iterator_tag;

            Iterator(Container& inner_container, Container& outer_container, std::function<void(Iterator*)> reset)
                : reset_inner([this, reset] { reset(this); }),
                  inner_container(inner_container), outer_container(outer_container),
                  inner(inner_container.begin()), outer(outer_container.begin()) {}

            Iterator& operator++() {
                ++inner;
                if (inner == inner_container.end()) {
                    ++outer;
                    if (outer != outer_container.end()) {
                        reset_inner();
                    }
                }
                return *this;
            }

            Iterator operator++(int) {
                Iterator temp = *this;
                ++(*this);
                return temp;
            }

            bool operator==(const Iterator& other) const {
                return inner == other.inner && outer == other.outer;
            }

            bool operator!=(const Iterator& other) const {
                return !(*this == other);
            }

            reference operator*() const {
                return {*inner, *outer};
            }

            reference operator*() {
                return {*inner, *outer};
            }

        protected:
            std::function<void()> reset_inner;
            Container& inner_container;
            Container& outer_container;
            ContainerIt inner;
            ContainerIt outer;
        };

        PairIterator(Container& container1, Container & container2, std::function<void(Iterator*)> reset):
            reset_inner(reset), inner_container(container1), outer_container(container2) {}

        Iterator begin() {
            return Iterator(inner_container, outer_container, reset_inner);
        }

        Iterator end() {
            Iterator it(inner_container, outer_container, reset_inner);
            it.outer = outer_container.end();
            it.inner = inner_container.end();
            return it;
        }

        Iterator begin() const {
            return Iterator(inner_container, outer_container, reset_inner);
        }

        Iterator end() const {
            Iterator it(inner_container, outer_container, reset_inner);
            it.outer = outer_container.end();
            it.inner = inner_container.end();
            return it;
        }

    private:
        std::function<void(Iterator*)> reset_inner;
        Container& inner_container;
        Container& outer_container;
    };


    // template <typename Container>
    // class CrossPairIterator {
    // public:
    //     class Iterator final : public PairIteratorBase<Container> {
    //     public:
    //         using PairIteratorBase<Container>::PairIteratorBase;
    //
    //     protected:
    //         void reset_inner() override {
    //             this->inner = this->inner_container.begin();
    //         }
    //     };
    //
    //     CrossPairIterator(Container& inner_container, Container& outer_container)
    //         : inner_container(inner_container), outer_container(outer_container) {}
    //
    //     Iterator begin() {
    //         return Iterator(inner_container, outer_container);
    //     }
    //
    //     Iterator end() {
    //         Iterator it(inner_container, outer_container);
    //         it.outer = outer_container.end();
    //         it.inner = inner_container.end();
    //         return it;
    //     }
    //
    // private:
    //     Container& inner_container;
    //     Container& outer_container;
    // };
    //
    //
    // template <typename Container>
    // class UniquePairIterator {
    // public:
    //     class Iterator final : public PairIteratorBase<Container> {
    //     public:
    //         using PairIteratorBase<Container>::PairIteratorBase;
    //
    //     protected:
    //         void reset_inner() override {
    //             this->inner = std::next(this->outer);
    //         }
    //     };
    //
    //     explicit UniquePairIterator(Container& container)
    //         : container(container) {}
    //
    //     Iterator begin() {
    //         Iterator it(container, container);
    //         if (it.outer != container.end()) {
    //             it.inner = std::next(it.outer); // make sure first begin is not a pair with two identical elements
    //             if (it.inner == container.end() && ++it.outer != container.end()) {
    //                 it.inner = std::next(it.outer);
    //             }
    //         }
    //         return it;
    //     }
    //
    //     Iterator end() {
    //         Iterator it(container, container);
    //         it.outer = container.end();
    //         it.inner = container.end();
    //         return it;
    //     }
    //
    // private:
    //     Container& container;
    // };
}
